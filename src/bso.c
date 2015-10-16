#include <pebble.h>
#define DELAY_MS 300

#define TEXT_HEIGHT 40

#ifdef PBL_PLATFORM_APLITE
  #define TOP_MARGIN 5
#else
  #define TOP_MARGIN 20
#endif

static Window *window;
static TextLayer *ball_layer;
static TextLayer *strike_layer;
static TextLayer *out_layer;
static TextLayer *timer_layer;

static int ball;
static int strike;
static int out;

static int history_count = 0;
#define MAX_TYPE    3
#define MAX_HISTORY 100
static int history[3][MAX_HISTORY];

static int s_uptime = 0;

/* プロトタイプ宣言 */
static void show_ball();
static void show_strike();
static void show_out();
static void four_ball();
static void struck_out();
static void threeout_change();

static void push(int type, int count)
{
  history_count++;
  if (history_count == 1) {
    if (s_uptime == 0) {
      s_uptime = 1;
      ball = 0;
      strike = 0;
      out = 0;
      text_layer_set_text(timer_layer, "Time: 0h:0m");      
    }
    for (int i = 0; i < MAX_TYPE; i++)
    {
        history[i][0] = history[i][0];
    }
    show_ball();
    show_strike();
    show_out();
  }
  if (history_count == 101) {
    history_count = 1;
  }
  for (int i = 0; i < MAX_TYPE; i++) {
    history[i][history_count] = history[i][history_count - 1];
  }
  history[type][history_count] = count;
}

static void show_ball()
{
  char *s;
  switch (ball) {
  case 0:
    s = "Ball 0";
    break;
  case 1:
    s = "Ball 1";
    break;
  case 2:
    s = "Ball 2";
    break;
  case 3:
    s = "Ball 3";
    break;
  case 4:
    s = "Ball 4";
    break;
  case 5:
    four_ball();
    s = "Ball 0";
    vibes_short_pulse();
    break;
  default:
    s = "Error!";
    break;
  }
  text_layer_set_text(ball_layer, s);
}

static void add_ball()
{
  ball++;
  push(0, ball);
  show_ball();
}

static void show_strike()
{
  char *s;
  switch (strike) {
  case 0:
    s = "Strike 0";
    break;
  case 1:
    s = "Strike 1";
    break;
  case 2:
    s = "Strike 2";
    break;
  case 3:
    s = "Strike 3";
    break;
  case 4:
    struck_out();
    s = "Strike 0";
    vibes_short_pulse();
    break;
  default:
    s = "Error!";
    break;
  }
  text_layer_set_text(strike_layer, s);
}

static void add_strike()
{
  strike++;
  push(1, strike);
  show_strike();
}

static void four_ball()
{
  strike = 0;
  history[1][history_count] = strike;
  show_strike();
  ball = 0;
  history[0][history_count] = ball;
}

static void struck_out()
{
  ball = 0;
  history[0][history_count] = ball;
  show_ball();

  strike = 0;
  history[1][history_count] = strike;

  out++;
  history[2][history_count] = out;
  show_out();
}

static void show_out()
{
  char *s;
  switch (out) {
  case 0:
    s = "Out 0";
    break;
  case 1:
    s = "Out 1";
    break;
  case 2:
    s = "Out 2";
    break;
  case 3:
    s = "Out 3";
    break;
  case 4:
    s = "Out 0";
    threeout_change();
    vibes_short_pulse();
    break;
  default:
    s = "Error!";
    break;
  }
  text_layer_set_text(out_layer, s);
}

static void add_out()
{
  out++;
  push(2, out);
  show_out();

  strike = 0;
  history[1][history_count] = strike;
  show_strike();

  ball = 0;
  history[0][history_count] = ball;
  show_ball();
}

static void threeout_change()
{
  ball = 0;
  history[0][history_count] = ball;
  show_ball();

  strike = 0;
  history[1][history_count] = strike;
  show_strike();

  out = 0;
  history[2][history_count] = out;

  history_count = 0;
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (history_count > 0) {
    history_count--;
    ball   = history[0][history_count];
    strike = history[1][history_count];
    out    = history[2][history_count];
    show_ball();
    show_strike();
    show_out();
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_ball();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_strike();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_out();
}

/* static void back_long_click_handler(ClickRecognizerRef recognizer, void *context) { */
/* } */

/* static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) { */
/* } */

/* static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) { */
/* } */

/* static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) { */
/* } */

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  /* window_long_click_subscribe(BUTTON_ID_BACK,   DELAY_MS, NULL, back_long_click_handler); */
  /* window_long_click_subscribe(BUTTON_ID_UP,     DELAY_MS, NULL, up_long_click_handler); */
  /* window_long_click_subscribe(BUTTON_ID_SELECT, DELAY_MS, NULL, select_long_click_handler); */
  /* window_long_click_subscribe(BUTTON_ID_DOWN,   DELAY_MS, NULL, down_long_click_handler); */
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (s_uptime > 0) {
    // Use a long-lived buffer
    static char s_uptime_buffer[32];

    // Get time since launch
    //int seconds = s_uptime % 60;
    int minutes = (s_uptime % 3600) / 60;
    int hours = s_uptime / 3600;

    // Update the TextLayer
    snprintf(s_uptime_buffer, sizeof(s_uptime_buffer), "Time: %dh %dm", hours, minutes);
    text_layer_set_text(timer_layer, s_uptime_buffer);

    // Increment s_uptime
    s_uptime += 60;
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  ball_layer = text_layer_create((GRect) { .origin = { 0, TOP_MARGIN }, .size = { bounds.size.w, TEXT_HEIGHT } });
  text_layer_set_text(ball_layer, "");
  text_layer_set_text_alignment(ball_layer, GTextAlignmentCenter);
  text_layer_set_font(ball_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  layer_add_child(window_layer, text_layer_get_layer(ball_layer));

  strike_layer = text_layer_create((GRect) { .origin = { 0, TOP_MARGIN + TEXT_HEIGHT }, .size = { bounds.size.w, TEXT_HEIGHT } });
  text_layer_set_text(strike_layer, "B S O");
  text_layer_set_text_alignment(strike_layer, GTextAlignmentCenter);
  text_layer_set_font(strike_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  layer_add_child(window_layer, text_layer_get_layer(strike_layer));

  out_layer = text_layer_create((GRect) { .origin = { 0, TOP_MARGIN + TEXT_HEIGHT * 2 }, .size = { bounds.size.w, TEXT_HEIGHT } });
  text_layer_set_text(out_layer, "");
  text_layer_set_text_alignment(out_layer, GTextAlignmentCenter);
  text_layer_set_font(out_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  layer_add_child(window_layer, text_layer_get_layer(out_layer));

  timer_layer = text_layer_create((GRect) { .origin = { 0, TOP_MARGIN + TEXT_HEIGHT * 3 }, .size = { bounds.size.w, 30 } });
  text_layer_set_text(timer_layer, "   Play Ball >> ");
  text_layer_set_text_alignment(timer_layer, GTextAlignmentCenter);
  text_layer_set_font(timer_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(timer_layer));

  ball       = 0;
  strike     = 0;
  out        = 0;
}

static void window_unload(Window *window) {
  text_layer_destroy(ball_layer);
  text_layer_destroy(strike_layer);
  text_layer_destroy(out_layer);
  text_layer_destroy(timer_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  // Subscribe to TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
