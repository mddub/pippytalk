#include <pebble.h>

#define LONG_CLICK_DELAY_MS 500

static Window *s_window;
static TextLayer *s_sound_text;
static TextLayer *s_clock_text;
static TextLayer *s_connected_text;

static time_t last_vibe_start = 0;

static void send_message(const char * message) {
  // Prevent this vibe from triggering a tap
  last_vibe_start = time(NULL);
  vibes_short_pulse();

  text_layer_set_text(s_sound_text, message);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, MESSAGE_KEY_sound, message);
  app_message_outbox_send();
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  if (time(NULL) - last_vibe_start > 2) {
    send_message("0");
  }
}

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message("1");
}

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message("2");
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message("3");
}

static void back_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message("4");
}

static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message("5");
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message("6");
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message("7");
}

static void click_config_provider(void *context) {
  accel_tap_service_subscribe(tap_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, back_single_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, LONG_CLICK_DELAY_MS, up_long_click_handler, NULL);
  window_long_click_subscribe(BUTTON_ID_SELECT, LONG_CLICK_DELAY_MS, select_long_click_handler, NULL);
  window_long_click_subscribe(BUTTON_ID_DOWN, LONG_CLICK_DELAY_MS, down_long_click_handler, NULL);
}

static void in_received_handler(DictionaryIterator *received, void *context) {
  text_layer_set_text(s_connected_text, "connected");
}

static void in_dropped_handler(AppMessageResult reason, void *context) {}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  text_layer_set_text(s_sound_text, "failed");
}

static void minute_handler(struct tm *tick_time, TimeUnits units_changed) {
  static char time_string[16];
  clock_copy_time_string(time_string, 16);
  // cut off " AM"/" PM"
  if (time_string[4] == ' ') {
    time_string[4] = 0;
  } else {
    time_string[5] = 0;
  }
  text_layer_set_text(s_clock_text, time_string);
}

static void window_load(Window *window) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  app_message_open(256, 256);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_sound_text = text_layer_create(GRect(0, 10, bounds.size.w, 32));
  text_layer_set_font(s_sound_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_sound_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_sound_text));

  s_clock_text = text_layer_create(GRect(0, bounds.size.h / 2 - 26, bounds.size.w, 50));
  text_layer_set_font(s_clock_text, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_clock_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_clock_text));

  s_connected_text = text_layer_create(GRect(0, bounds.size.h - 25, bounds.size.w, 32));
  text_layer_set_font(s_connected_text, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_connected_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_connected_text));

  tick_timer_service_subscribe(MINUTE_UNIT, minute_handler);
}

static void window_unload(Window *window) {
  text_layer_destroy(s_sound_text);
  text_layer_destroy(s_clock_text);
  text_layer_destroy(s_connected_text);
}

static void init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
