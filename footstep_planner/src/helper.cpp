// SVN $HeadURL$
// SVN $Id$

/*
 * A footstep planner for humanoid robots
 *
 * Copyright 2010-2011 Johannes Garimort, Armin Hornung, University of Freiburg
 * http://www.ros.org/wiki/footstep_planner
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <footstep_planner/helper.h>

namespace footstep_planner
{
    void get_footstep(
            double from_x, double from_y, double from_theta, Leg from_leg,
            double to_x, double to_y, double to_theta,
            double& footstep_x, double& footstep_y, double& footstep_theta)
    {
        footstep_theta = angles::shortest_angular_distance(from_theta,
                                                           to_theta);

        double theta_cos = cos(-from_theta);
        double theta_sin = sin(-from_theta);
		to_x -= from_x;
		to_y -= from_y;
        if (from_leg == RIGHT)
        {
            footstep_x = theta_cos * to_x - theta_sin * to_y;
            footstep_y = theta_sin * to_x + theta_cos * to_y;
        }
        else
        {
            footstep_x =  theta_cos * to_x - theta_sin * to_y;
            footstep_y = -theta_sin * to_x - theta_cos * to_y;
            footstep_theta = -footstep_theta;
        }
    }


	bool
	collision_check(double x, double y, double theta, double height,
                    double width, int accuracy, const GridMap2D& distance_map)
	{
		double d = distance_map.distanceMapAt(x, y);
		if (d < 0.0) // if out of bounds => collision
			return true;
		d -= distance_map.getResolution();

		double r_o = sqrt(width*width + height*height) / 2.0;

		if (d >= r_o)
			return false;
		else if (accuracy == 0)
			return false;

		double h_half = height / 2.0f;
		double w_half = width / 2.0f;
		double r_i = std::min(w_half, h_half);

		if (d <= r_i)
			return true;
		else if (accuracy == 1)
			return true;

		double h_new;
		double w_new;
		double delta_x;
		double delta_y;
		if (width < height)
		{
			double h_clear = sqrt(d*d - w_half*w_half);
			h_new = h_half - h_clear;
			w_new = width;
			delta_x = h_clear + h_new/2.0;
			delta_y = 0.0;
		}
		else // footWidth >= footHeight
		{
			double w_clear = sqrt(d*d - h_half*h_half);
			h_new = height;
			w_new = w_half - w_clear;
			delta_x = 0.0;
			delta_y = w_clear + w_new/2.0;
		}
		double theta_cos = cos(theta);
		double theta_sin = sin(theta);
		double x_shift = theta_cos*delta_x - theta_sin*delta_y;
		double y_shift = theta_sin*delta_x + theta_cos*delta_y;

		return (collision_check(x+x_shift, y+y_shift, theta, h_new, w_new,
		                        accuracy, distance_map) ||
				collision_check(x-x_shift, y-y_shift, theta, h_new, w_new,
                                accuracy, distance_map));
	}


	bool
	State::operator ==(const State& s2)
	{
		return (fabs(x - s2.x) <= FLOAT_CMP_THR &&
		        fabs(y - s2.y) <= FLOAT_CMP_THR &&
		        fabs(theta - s2.theta) <= FLOAT_CMP_THR &&
		        leg == s2.leg);
	}


	bool
	State::operator !=(const State& s2)
	{
		return not (*this == s2);
	}
}
