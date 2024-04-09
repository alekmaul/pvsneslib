#ifndef SUPERSCOPE_H
#define SUPERSCOPE_H

#define normal_fire_rate 10 // To put some limit to turbo mode, so we're gonna fire every 10 frames
#define slow_fire_rate 20 // To put some limit to turbo mode, so we're gonna fire every 20 frames

#define max_bullets 32
#define max_targets 64

bool sscope_disconnected = false; // if SuperScope is disconnected during gameplay, and plugged again, we can control some behavior
bool pause_adjust = false;        // If we pause during game, will lead us to calibration screen, but we will be able to continue our game

// Let's declare some usefull variables to animate our fantastic blue bullets :D
u8 ready_to_fire = 0;
u8 cool_down = 0;
u8 bullet_id = 0;    // bullet id, we're going to use a maximum of 32 bullets, and then they reset to 0
u8 bullet_num = 0;   // bullet number, this will help us to draw multiple bullets on screen at the same time
u8 shot_bullets = 0; // the number of bullets we have shot and need to be drawn on screen. Every time a bullet dissapears, this number will decrease by one.

u16 bullet_x[max_bullets];
u8 bullet_y[max_bullets];

s16 difuse_xn[max_bullets] = {0};
s16 difuse_yn[max_bullets] = {0};
s8 difuse_x[max_bullets] = {0};
s8 difuse_y[max_bullets] = {0};
u8 bullet_frame[max_bullets] = {0};
u8 bullet_draw[max_bullets] = {0};
u8 bullet_frame_n[max_bullets] = {0};
u8 bullet_gravity[max_bullets] = {0};
u8 bullet_diff[max_bullets] = {1};

u8 bullet_frames[] = {0, 4, 8, 12, 64, 68, 72, 76}; // GFX positions for the OAM table

bool enable_fire = false;
bool enable_pause = false;
bool enable_cursor = false;

// And out targets
u8 target_id = 0;
u8 target_oam_id[max_targets];

u16 target_x_inc[max_targets];
u16 target_y_inc[max_targets];

u16 target_x[max_targets];
u8 target_y[max_targets];
s8 target_collision_x[max_targets];
s8 target_collision_y[max_targets];

bool target_kill[max_targets] = {false};
u8 target_kill_count[max_targets] = {0};
u8 target_gravity[max_targets] = {0};
u8 targets_shot = 0;

#endif