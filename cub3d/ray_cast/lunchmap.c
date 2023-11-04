/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lunchmap.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otamrani <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 16:30:44 by otamrani          #+#    #+#             */
/*   Updated: 2023/11/04 15:44:15 by otamrani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "cub3d.h"
// 1111111111111111111
// 1000000000000100001
// 1111111111110N11101
// 1000000000000000101
// 1111111111111111101
// 1000000000000000001
// 1011111110011111101
// 1010000010010000001
// 1111011111111111111
// 1000000000000000001
// 1111111111111111111
void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char *dst;
    int rect_x;
    int rect_y;
    int y1 = (900 / data->all->y_of_map) *0.2;
    int x1 = (1300 / data->all->x_of_map) *0.2;
    rect_x = x * x1;
    rect_y = y * y1;
    int i = rect_x;
    int j = rect_y;
    while(j < rect_y + y1)
    {
        i = rect_x;
        while(i < rect_x + x1)
        {
            dst = data->addr + (j * data->line_length + i * (data->bits_per_pixel / 8));
            *(unsigned int *)dst = color;
            i++;
        }
        j++;
    }
    
}
void draw_line(t_data *data, double x_intercept, double y_intercept)
{

    // Assuming that mlx_ptr and win_ptr are properly initialized
    char *dst;
    double dx = x_intercept - (data->pos_x *0.2);
    double dy = y_intercept - (data->pos_y*0.2 );
    double steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
    double xIncrement = dx / steps;
    double yIncrement = dy / steps;
    double x = data->pos_x *0.2;
    double y = data->pos_y*0.2 ;

    for (int i = 0; i < steps; i++) {
        if(x < 0 || y < 0 || x >= 1300 || y >= 900)
            break;
         dst = data->addr + ((int)(y)  * data->line_length + ((int)x) * (data->bits_per_pixel / 8));
             *(unsigned int *)dst = 0x0000FF;
        x += xIncrement;
        y += yIncrement;
    }
}
int getupdown(double direction)
{
    if(direction >= 0 && direction <= M_PI)
        return(1);
    return(2);
}

int getFacingDirection(double direction) {
    if(direction <= M_PI /2 || direction >= 2 * M_PI - M_PI / 2)
            return(1);
    return(2);
}



void vert_cord(t_data *data, double direction)
{
    double x_intercept;
    double y_intercept;
    double distancev;
    double distanceh;
    double x_step;
    double y_step;
    double ray_angle;
    data->found_wallv = 0;
    ray_angle = direction;
    x_intercept = floor(data->pos_x / (1300 / data->all->x_of_map)) * (1300 / data->all->x_of_map);
    x_intercept += (getFacingDirection(ray_angle) == 1) ? (1300 / data->all->x_of_map) : -0.00000001;
    y_intercept = data->pos_y + (x_intercept - data->pos_x) * tan(ray_angle);
    x_step = (1300 / data->all->x_of_map);
    // printf("%f\n", x_step);
    // printf("getFacingDirection in ver= %d\n", getFacingDirection(ray_angle));
    x_step *= (getFacingDirection(ray_angle) == 2) ? -1 : 1;
    // printf("%f\n", x_step);
    y_step = (1300 / data->all->x_of_map) * tan(ray_angle);
    // printf("getupdow in ver= %d\n", getupdown(ray_angle));
    y_step *= (getupdown(ray_angle) == 2 && y_step > 0) ? -1 : 1;
    y_step *= (getupdown(ray_angle) == 1 && y_step < 0) ? -1 : 1;
    if(getFacingDirection(ray_angle) == 2)
        x_intercept -= 0.00000001;
    while (y_intercept >= 0 && y_intercept < 900 && x_intercept >= 0 && x_intercept < 1300)
    {
        int x = x_intercept / (1300 / data->all->x_of_map);
        int y = y_intercept / (900 / data->all->y_of_map);
        if ((y < data->all->y_of_map && x < data->all->x_of_map && data->all->map[y][x] == '1'))
        {
            // printf("x in = %d y in = %d\n", x, y);
            // printf("x_step = %f y_step = %f\n", x_step, y_step);
            data->found_wallv = 1;
            break;
        }
        y_intercept += y_step;
        x_intercept += x_step;
    }
    data->x_ver = x_intercept;
    data->y_ver = y_intercept;
    distanceh = (data->found_wallh == 1) ? sqrt(pow(data->x_hor - data->pos_x, 2) + pow(data->y_hor - data->pos_y, 2)) : INT_MAX;
    distancev = (data->found_wallv == 1) ? sqrt(pow(data->x_ver - data->pos_x, 2) + pow(data->y_ver - data->pos_y, 2))  : INT_MAX;
    if(distanceh < distancev)
    {
        // printf("---hor---\n");
        // printf("%f\n", data->found_wallh);
        data->wall_hit_x = data->x_hor;
        data->wall_hit_y = data->y_hor;
        data->distance = distanceh;
    }
    else
    {
        // printf("---ver---\n");
        data->wall_hit_x = data->x_ver;
        data->wall_hit_y = data->y_ver;
        data->distance = distancev;
    }
    data->distance *= cos(data->direction - direction);
    // draw_line(data, data->wall_hit_x * 0.2, data->wall_hit_y * 0.2);
}
void draw_ray(t_data *data, double direction)
{
    double ray_angle;
    double y_intercept;
    double x_intercept;
    double x = data->pos_x;
    double y = data->pos_y;
    double x_step;
    double y_step;
    data->found_wallh = 0;
    char *dst;
    ray_angle = direction;
    y_intercept = floor(data->pos_y / (900 / data->all->y_of_map)) * (900 / data->all->y_of_map);
    if(getupdown(ray_angle) == 1)
        y_intercept += (900 / data->all->y_of_map);
    if(getupdown(ray_angle) == 2)
        y_intercept -= 0.000001;
    x_intercept = data->pos_x + (y_intercept - data->pos_y) / tan(ray_angle);
    // if (ray_angle == 0)
    //     x_intercept = 1300 / data->all->x_of_map;
    y_step = (900/ data->all->y_of_map);
    // printf("getupdown = %d\n", getupdown(ray_angle));
    // printf("{%f}\n",round(ray_angle));
    if(getupdown((ray_angle)) == 2)
        y_step *= -1;
    // y_step *= (ray_angle > M_PI) ? -1 : 1;
    x_step = (900 / data->all->y_of_map)/ tan(ray_angle);
    // if (ray_angle == 0)
    //     x_step = (1300 / data->all->x_of_map);
    // else if(ray_angle == M_PI)
    //     x_step = -1 * (1300 / data->all->x_of_map);
    if(getFacingDirection(ray_angle) == 2 && x_step > 0)
    {
        x_step *= -1;
    }
    else if(getFacingDirection(ray_angle) == 1 && x_step < 0)
        x_step *= -1;
    // printf("y_step = %f y_intercept = %f\n", y_step, y_intercept);
    // printf("data->direc = %f\n", ray_angle);
    while (y_intercept >= 0 && y_intercept < 900 && x_intercept >= 0 && x_intercept < 1300)
    {
        int x = x_intercept / (1300 / data->all->x_of_map);
        int y = y_intercept / (900 / data->all->y_of_map);
        if ((y < data->all->y_of_map && x < data->all->x_of_map && data->all->map[y][x] == '1'))
        {
            data->found_wallh = 1;
            break;
        }
        y_intercept += y_step;
        x_intercept += x_step;
    }
    //  if(x_intercept < 0)
    // {
    //      x_intercept = 0;
    // }
    // if(y_intercept < 0)
    //         y =0 ;
    data->x_hor = x_intercept;
    data->y_hor = y_intercept;
    // printf("%f && %f\n", round(x_intercept / (1300 /data->all->x_of_map)), round(y_intercept / (900 / data->all->y_of_map)));
    // printf("%f\n", floor(y_intercept));
    vert_cord(data, direction);
}
unsigned int where_is_pixl(t_data *data, int x , int y)
{
    data->x_to_check = (int)data->wall_hit_x / 65;
    data->y_to_check = (int)data->wall_hit_y / 90;
    printf("wall_hit_x = %f wall_hit_y = %f\n", data->wall_hit_x, data->wall_hit_y);
    // printf("x_text = %d y_text = %d\n", x_text, y_text);
    // // exit(0);
    //return(data->txt->img_data[y_text * (1300 / data->all->x_of_map) + x_text]);
    return(0x000000);
    // exit(0);
}
void draw_wall(t_data *data)
{
    int y_start;
    int x_srart = 0;
    char *dst;

   while(x_srart < 1300)
   {
    data->dist_proj_plane = (900 / 2) / tan(data->fov / 2);
    data->wall_height = ((900 / 10) / data->all_rays[x_srart]) * data->dist_proj_plane;
    y_start = (900 / 2) - (data->wall_height / 2);
    if(y_start < 0)
        y_start = 0;
    if(data->wall_height > 900)
        data->wall_height = 900;
       while(y_start < (900 / 2) + (data->wall_height / 2))
       {
            dst = data->addr + (y_start * data->line_length + x_srart * (data->bits_per_pixel / 8));
            *(unsigned int *)dst =  0X000000;
            y_start++;
       }
       x_srart++;
   }
//    while(data->y_to_check < 10)
    printf("data->wall_x %d wall_y %d\n", (int)data->wall_hit_x % 65, (int)data->wall_hit_y % 90);
    printf("data->wall_x sq %d wall_y sq %d\n", (int)data->wall_hit_x / 65, (int)data->wall_hit_y / 90);
       {
    dst = data->addr + (((int)data->wall_hit_x ) * data->line_length + ((int)data->wall_hit_y ) * (data->bits_per_pixel / 8));
    *(unsigned int *)dst =  0xFFFFFF;
   }
}


void draw_map(t_data *data)
{
    int x = 0;
    int i = 0;
    char *dst;
    int y = 0;
    int t = 0;
    unsigned int sky;
    unsigned int gorund;
    sky = data->all->txt->F_CLOR;
    gorund = data->all->txt->C_CLOR;
    data->fov = 60 * (M_PI / 180);
    data->ray_angle = data->direction - (data->fov / 2);
     data->all_rays = malloc(sizeof(double) * 1300);
    while (t < 1300 * 900)
    {
        char *dst;
        dst = data->addr + (t * (data->bits_per_pixel / 8));
        if(t < 450 * 1300)
            *(unsigned int *)dst = sky;
        else
            *(unsigned int *)dst = gorund;
            t++;
    }
    while(i < 1300)
    {
    if(data->ray_angle > 2 * M_PI)
        data->ray_angle = data->ray_angle - 2 * M_PI;
    draw_ray(data, data->ray_angle);
    data->ray_angle += data->fov / 1300;
    data->all_rays[i] = data->distance;
    i++;
    }
    draw_wall(data);
    int color;
//    while(y < data->all->y_of_map)
//    {
//        x = 0;
//        while(x < data->all->longest_line)
//        {
//            color = (data->all->map[y][x] == '1') ? 0x000000 : 0xFFFFFF;
//            my_mlx_pixel_put(data, x, y, color);
//            x++;
//        }
//        y++;
//    }
//     color = 0x000000;
//     dst = data->addr + ((int)(data->pos_y * 0.2) * data->line_length + ((int)(data->pos_x * 0.2) * (data->bits_per_pixel / 8)));
//              *(unsigned int *)dst = color;
//     i  = 0;
//     data->fov = 60 * (M_PI / 180);
//     data->ray_angle = data->direction - (data->fov / 2);
    // while(i < 1300)
    // {
    // if(data->ray_angle > 2 * M_PI)
    //     data->ray_angle = data->ray_angle - 2 * M_PI;
    // draw_ray(data, data->ray_angle);
    // data->ray_angle += data->fov / 1300;
    // data->all_rays[i] = data->distance;
    // i++;
    // }
    mlx_put_image_to_window(data->mlx_ptr, data->mlx_win, data->img, 0, 0);
}
int player_collision(t_data *data)
{
    int x = data->pos_x;
    int y = data->pos_y;
    int y1 = (900 / 10);
    int x1 = (1300 / 20);
    // printf("first %c\n", data->all->map[y / y1][x / x1]);
    // printf("twoo %c\n", data->all->map[(y + 10) / y1][(x + 10) / x1]);
    // printf("three %c\n", data->all->map[(y + 10) / y1][x / x1]);
    // printf("four %c\n", data->all->map[y / y1][(x + 10) / x1]);
    // if(data->all->map[y / y1][x / x1] == '1')
    //     return(1);
    // else if(data->all->map[(y + 1) / y1][(x + 1) / x1] == '1')
    //     return(1);
    // else if(data->all->map[(y + 1) / y1][x / x1] == '1')
    //         return(1);
    // else if(data->all->map[y / y1][(x + 1) / x1] == '1')

    //             return(1);
    return(0);
}
int move_player(t_data *data)
{
    double direction;
    double x = data->pos_x;
    double y = data->pos_y;
    if(data->key_a == 1)
    {
        data->pos_x -= cos(data->direction + M_PI_2) * 2;
        data->pos_y  -= sin(data->direction + M_PI_2) * 2;
    }
    if(data->key_d == 1)
    {
        data->pos_x += cos(data->direction + M_PI_2) * 2;
        data->pos_y += sin(data->direction + M_PI_2) * 2;
    }
    else if(data->key_w  == 1 || data->key_s == 1)
    {
        direction = data->direction;
        if(data->key_s == 1)
            direction = data->direction + M_PI;
        data->pos_y += sin(direction) * 2;
        data->pos_x += cos(direction) * 2;
    }
    else if(data->key_left == 1)
    {
        data->direction -= 0.02;
        if(data->direction < 0)
            data->direction = 2 * M_PI + data->direction;
    }
    else if(data->key_right == 1)
    {
        data->direction += 0.02;
        if(data->direction > 2 * M_PI )
            data->direction = data->direction - 2 * M_PI;
    }
  if(player_collision(data) == 1)
    {
        data->pos_x = x;
        data->pos_y = y;
        return (0);
    }
    draw_map(data);
    return (0);
}

void find_player_position(t_all *all, t_data *data) {
    int y = 0;
    int x;
    while (y < all->y_of_map) {
        x = 0;
        while (x < all->longest_line) {
            if (all->map[y][x] == 'N') {
                data->pos_x = x * (1300 / all->x_of_map);
                data->pos_y = y * (900 / all->y_of_map);
                return; 
            }
            
            x++;
        }
        y++;
    }
}
int press_key(int key, t_data *data)
{
    if(key == KEY_A)
        data->key_a = 1;
    if(key == KEY_D)
        data->key_d = 1;
    if(key == KEY_W)
        data->key_w = 1;
    if(key == KEY_S)
        data->key_s = 1;
    if(key == KEY_LEFT)
        data->key_left = 1;
    if(key == KEY_RIGHT)
        data->key_right = 1;
    return(0);
}

int release_key(int key, t_data *data)
{
    if(key == KEY_A)
        data->key_a = 0;
    if(key == KEY_D)
        data->key_d = 0;
    if(key == KEY_W)
        data->key_w = 0;
    if(key == KEY_S)
        data->key_s = 0;
    if(key == KEY_LEFT)
        data->key_left = 0;
    if(key == KEY_RIGHT)
        data->key_right = 0;
    return(0);
}
void	keys_init(t_data *data)
{
	data->key_a = 0;
	data->key_s = 0;
	data->key_d = 0;
	data->key_w = 0;
	data->key_left = 0;
	data->key_right = 0;
}
int close_window(t_data *data)
{
    mlx_destroy_window(data->mlx_ptr, data->mlx_win);
    exit(0);
}
void show_map(t_all *all, t_textr *txt)
{
    t_data *data;
    data = malloc(sizeof(t_data));
    data->mytable = malloc(sizeof(double) * (1300 * 2));
    data->all = all;
    data->txt = txt;
    all->x_of_map = all->longest_line;
    data->direction =  M_PI_2;
    find_player_position(all, data);
    keys_init(data);
    printf("%d\n", data->all->y_of_map);
    data->mlx_ptr = mlx_init();
    data->mlx_win = mlx_new_window(data->mlx_ptr, 1300,  900, "CUB3D");
    data->img = mlx_new_image(data->mlx_ptr, 1300, 900);
	data->addr = mlx_get_data_addr(data->img, &data->bits_per_pixel, &data->line_length,
								&data->endian);
    draw_map(data);
    mlx_hook(data->mlx_win, 17, 0L, close_window, data);
    mlx_hook(data->mlx_win, 2, 1L << 0, press_key, data);
    mlx_hook(data->mlx_win, 3, 1L << 1, release_key, data);
    mlx_loop_hook(data->mlx_ptr, move_player, data);
    mlx_loop(data->mlx_ptr);
}
// note to do ...
// 1, player in other direction  
// 2, optimise the width and height of map
// 3 data->all->y_ofmap not work with mlx_loop_hook