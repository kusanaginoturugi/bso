#define DELAY_MS    300
#define TEXT_HEIGHT 40
#define MAX_TYPE    3
#define MAX_HISTORY 100

#ifdef PBL_PLATFORM_APLITE
  #define TOP_MARGIN 5
#else
  #define TOP_MARGIN 20
#endif

static Window    *window;
static TextLayer *inning_layer;
static TextLayer *ball_layer;
static TextLayer *strike_layer;
static TextLayer *out_layer;
static TextLayer *timer_layer;

// 0,1->1 2,3->2 4,5->3 6,7->4 8,9->5
// 10,11->6 12,13->7 14,15->8 16,17->9 18,19->10
static int inning_count = 0;
static int ball;
static int strike;
static int out;
static int history_count = 0;
static int history[3][MAX_HISTORY];
static int s_uptime = 0;
static char inning_s[10] = "top 1st";

/* Function Prototype */
static void show_ball();
static void show_strike();
static void show_out();
static void four_ball();
static void struck_out();
static void threeout_change();
static void update_textlayer();
