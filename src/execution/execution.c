/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: naadam <naadam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 19:37:03 by naadam            #+#    #+#             */
/*   Updated: 2024/08/31 14:45:31 by naadam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

void	set_plane(t_player *player)
{
	player->plane_length = tan(PI / 6); // Plane length is fixed
	//Set camera plane
	player->plane_x = player->dir_y * player->plane_length; 
	player->plane_y = (player->dir_x * -1) * player->plane_length;
}

void	set_direction(t_player *player)
{
	if (player->angle == 0) // EAST
	{
		player->dir_x = 1;
		player->dir_y = 0;
	}
	else if (player->angle == PI / 2) // NORTH
	{
		player->dir_x = 0;
		player->dir_y = 1;
	}
	else if (player->angle == PI) // WEST
	{
		player->dir_x = -1;
		player->dir_y = 0;
	}
	else if (player->angle == 3 * PI / 2) // SOUTH
	{
		player->dir_x = 0;
		player->dir_y = -1;
	}
}

void	cal_side(t_player *player, t_map *map)
{
	map->mapX = (int)player->pos_x;
	map->mapY = (int)player->pos_y;
	if (player->raydir_x < 0)
	{
    	player->stepX = -1; //Stepping to the left side
		player->sideDistX = (player->pos_x - map->mapX) * player->deltaDistX; // Distance to a left border of a grid
	}
	else
	{
		player->stepX = 1; // Stepping to the right side
		player->sideDistX = (map->mapX + 1.0 - player->pos_x) * player->deltaDistX; // Distance to a right border of a grid
	}
	if (player->raydir_y < 0)
	{
		player->stepY = -1; // Stepping down
		player->deltaDistY = (player->pos_y - map->mapY) * player->deltaDistY; // Distance to the border below
	}
	else
	{
		player->stepY = 1; // Stepping up
		player->deltaDistY = (map->mapY + 1.0 - player->pos_y) * player->deltaDistY; // Distance to the upperside
	}
}

void	performDDA(t_player *player, t_map *map, t_parse *p)
{
	int	hit;

	hit	= 0;
	while (hit == 0)
	{
		if (player->sideDistX < player->sideDistY) //If x is less than y we move vertically
		{
			player->sideDistX += player->deltaDistX; // We add onto the side distance using deltaDistX
			map->mapX += player->stepX; // We step in a vertical direction
			player->side = 0; // Since it touches the x-plane we're going to set it to zero
		}
		else
		{
			player->sideDistY += player->deltaDistY; // We move horizontally
			map->mapY += player->stepY; // We step in a horizontal direction
			player->side = 1; // Since it touches the y-plane we're going to set it to one
	 	}
		if (map->layout[map->mapY / p->tile_size][map->mapX / p->tile_size] == '1') // Divide by tile_size if just to align our tiles and pixels
			hit = 1; // Will leave loop since wall has been hit
	}
}

void	cal_delta(t_player *player)
{
	player->deltaDistX = fabs(1 / player->raydir_x); // Difference between two vertical intersections
	player->deltaDistY = fabs(1 / player->raydir_y); // Difference between two horizontal intersections
}

void	calWallDist(t_player *player, t_parse *p)
{
	(void)p;
	if (player->side == 0)
		player->perpWallDist = player->sideDistX - player->deltaDistX;
	else
		player->perpWallDist = player->sideDistY - player->deltaDistY;
	// printf("Perpetual wall distance: %f\n", player->perpWallDist);
	// player->endRayX = (player->pos_x + player->raydir_x) * player->perpWallDist;
	// player->endRayY = (player->pos_y + player->raydir_y) * player->perpWallDist;
	// if (player->endRayX < 0)
	// 	player->endRayX = 0;
	// if (player->endRayX >= M_WIDTH)
	// 	player->endRayX = M_WIDTH - 1;
	// printf("Ray end-point  at x %f Ray end-point at y %f\n", player->endRayX, player->endRayY);
}

void	set_raydir(t_player *player, t_window *window, t_data *m)
{
	int	x;

	x = 0;
	(void)window;
	while (x < M_WIDTH)
	{
		player->cameraX = (2 * x / (double)M_WIDTH) - 1; // Calculate cameraX for each column
		player->raydir_x = player->dir_x + player->plane_x * player->cameraX; // Calculation of raydir_x
		player->raydir_y = player->dir_y + player->plane_y * player->cameraX; // Calculation of raydir_y
		// Raycasting to take place here
		cal_side(player, m->map);
		cal_delta(player);
		performDDA(player, m->map, m->parse);
		calWallDist(player, m->parse);
		x++;
	}
}

void	raycasting(t_data *main)
{
	set_direction(main->player); // Player direction is being set
	set_plane(main->player); // Set where the player is looking
	set_raydir(main->player, main->window, main); 
}

void	draw_minimap(t_data *m)
{
	draw_grid(m); //Drawing the grid the player will be on 
	draw_player(m->window, m->player, m->parse->tile_size);//Drawing the player
	raycasting(m);  //Function for drawing the rays
	mlx_put_image_to_window(m->window->mlx, m->window->window, m->window->img, 0, 0);
	mlx_hook(m->window->window, 02, 1L << 0, keypress, m); // Keyboard is handled
	mlx_hook(m->window->window, 17, 1L << 0, exit_window, m); // Close button is handled
}

void    execution(t_data *main)
{
	// printf("%c\n", main->map->layout[12][26]);
	main->window = malloc(sizeof(t_window));
	main->player = malloc(sizeof(t_player));
	draw_minimap(main);
	mlx_loop(main->window->mlx);
} 