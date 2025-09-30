import os, sys, io
import M5
from M5 import *
import time
from boot_option import *
from hardware import Button
from hardware import Rotary



logo_r_image = None
big_number_text = None
page_title = None
pushtext_bg = None
push_text = None
down_arrow = None
small_blind_active = None
elapsed_mins = None
big_blind_active = None
mins_label = None
elapsed_secs = None
secs_label = None
active_small_blind_label = None
active_big_blind_label = None
Btn46 = None
rotary = None


import math

sound_notes = None
current_screen = None
sound_notes_list = None
current_round = None
round_secs_remaining = None
sound_round = None
rotary_increment = None
minutes_between_rounds = None
small_blind_value = None
last_timestamp = None

# Describe this function...
def set_sound_map():
  global sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp, logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary
  sound_notes = {'G#6':1661,'C7':2093,'C#7':2217,'D7':2349,'D#7':2489,'E7':2637,'F7':2793,'F#7':2959,'G7':3135,'G#7':3322,'A7':3520,'A#7':3729,'B7':3951,'C8':4186}
  sound_notes_list = [2093, 2217, 2349, 2489, 2637, 2793, 2959, 3135, 3322, 3520, 3729, 3951, 4186]

# Describe this function...
def small_blind_screen():
  global sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp, logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary
  print("small_blind_screen")
  current_screen = 1
  Widgets.fillScreen(0x000000)
  page_title.setText(str('Starting small blinds'))
  page_title.setVisible(True)
  small_blind_value = 25
  big_number_text.setText(str(small_blind_value))
  big_number_text.setVisible(True)
  pushtext_bg.setVisible(True)
  push_text.setVisible(True)
  down_arrow.setVisible(True)

# Describe this function...
def round_mins_screen():
  global sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp, logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary
  current_screen = 2
  Widgets.fillScreen(0x000000)
  page_title.setText(str('Mins between rounds'))
  page_title.setVisible(True)
  minutes_between_rounds = 10
  big_number_text.setText(str(minutes_between_rounds))
  big_number_text.setVisible(True)
  pushtext_bg.setVisible(True)
  push_text.setVisible(True)
  down_arrow.setVisible(True)

# Describe this function...
def round_underway_screen():
  global sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp, logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary
  current_screen = 3
  Widgets.fillScreen(0x000000)
  page_title.setCursor(x=77, y=25)
  page_title.setText(str((str('Round ') + str(current_round))))
  small_blind_active.setText(str(small_blind_value))
  big_blind_active.setText(str(2 * small_blind_value))
  active_small_blind_label.setVisible(True)
  active_big_blind_label.setVisible(True)
  mins_label.setVisible(True)
  secs_label.setVisible(True)
  last_timestamp = time.ticks_ms()

# Describe this function...
def check_round_end():
  global sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp, logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary
  if round_secs_remaining <= 0:
    current_round = (current_round if isinstance(current_round, (int, float)) else 0) + 1
    sound_round = (sound_round if isinstance(sound_round, (int, float)) else 0) + 1
    if sound_round > 10:
      sound_round = 10
    page_title.setText(str((str('Round ') + str(current_round))))
    small_blind_value = small_blind_value * 2
    # Hardcoded an absolute max as a failsafe
    if small_blind_value > 9999:
      small_blind_value = 9999
    small_blind_active.setText(str(small_blind_value))
    big_blind_active.setText(str(2 * small_blind_value))
    Speaker.tone(sound_notes_list[int(sound_round - 1)], 500)
    time.sleep_ms(600)
    Speaker.tone(sound_notes_list[int((sound_round + 1) - 1)], 500)
    time.sleep_ms(600)
    Speaker.tone(sound_notes_list[int((sound_round + 2) - 1)], 500)
    time.sleep_ms(600)
    Speaker.tone(sound_notes_list[int((sound_round + 3) - 1)], 500)
    round_secs_remaining = 60 * minutes_between_rounds

# Describe this function...
def play_affirm_sound():
  global sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp, logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary
  Speaker.tone(sound_notes['G7'], 150)
  time.sleep_ms(150)
  Speaker.tone(sound_notes['C8'], 150)
  time.sleep_ms(1000)


def btnA_wasClicked_event(state):
  global logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary, sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp
  print(f"BtnA clicked, current screen: {current_screen}")
  if current_screen==1:
    round_mins_screen()
    play_affirm_sound()
  elif current_screen==2:
    round_secs_remaining = 60 * minutes_between_rounds
    current_round = 1
    sound_round = 1
    round_underway_screen()
    play_affirm_sound()
  elif current_screen==3:
    print("BtnA on screen 3: turning off")
    from machine import Pin
    hold_pin = Pin(46, Pin.OUT)
    hold_pin.value(0)
    Btn46 = Button(46, active_low=True, pullup_active=True)
  else:
    pass


def setup():
  global logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary, sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp

  M5.begin()
  Widgets.fillScreen(0x000000)
  logo_r_image = Widgets.Image("/flash/res/img/riccy.jpeg", -1, 10, scale_x=1, scale_y=1)
  big_number_text = Widgets.Label("XX", 79, 100, 1.0, 0xff00dc, 0x000000, Widgets.FONTS.DejaVu56)
  page_title = Widgets.Label("Page title goes here", 20, 62, 1.0, 0x9a9a9a, 0x000000, Widgets.FONTS.DejaVu18)
  pushtext_bg = Widgets.Rectangle(-3, 190, 251, 62, 0x000000, 0xdf7b0f)
  push_text = Widgets.Label("Push", 97, 202, 1.0, 0x000000, 0xdf7b0f, Widgets.FONTS.DejaVu18)
  down_arrow = Widgets.Triangle(119, 234, 100, 225, 140, 225, 0x000000, 0x000000)
  small_blind_active = Widgets.Label("8888", 88, 58, 1.0, 0x00ff46, 0x000000, Widgets.FONTS.DejaVu56)
  elapsed_mins = Widgets.Label("MM", 94, 191, 1.0, 0xffffff, 0x000000, Widgets.FONTS.DejaVu24)
  big_blind_active = Widgets.Label("8888", 71, 121, 1.0, 0x00fbff, 0x000000, Widgets.FONTS.DejaVu56)
  mins_label = Widgets.Label("mins", 90, 218, 1.0, 0x9a9a9a, 0x000000, Widgets.FONTS.DejaVu12)
  elapsed_secs = Widgets.Label("SS", 137, 202, 1.0, 0xffffff, 0x000000, Widgets.FONTS.DejaVu12)
  secs_label = Widgets.Label("secs", 129, 218, 1.0, 0x9a9a9a, 0x000000, Widgets.FONTS.DejaVu12)
  active_small_blind_label = Widgets.Label("Small blind", 5, 82, 1.0, 0x00ff46, 0x000000, Widgets.FONTS.DejaVu12)
  active_big_blind_label = Widgets.Label("Big blind", 5, 143, 1.0, 0x00fbff, 0x000000, Widgets.FONTS.DejaVu12)

  BtnA.setCallback(type=BtnA.CB_TYPE.WAS_CLICKED, cb=btnA_wasClicked_event)

  set_boot_option(0)
  Widgets.fillScreen(0x000000)
  Btn46 = Button(46, active_low=False, pullup_active=True)
  from machine import Pin
  hold_pin = Pin(46, Pin.OUT)
  hold_pin.value(1)
  rotary = Rotary()
  set_sound_map()
  current_screen = 0
  logo_r_image.setVisible(True)
  Speaker.tone(sound_notes['G7'], 200)
  Speaker.setVolumePercentage(0.5)
  time.sleep_ms(150)
  Speaker.tone(sound_notes['C8'], 200)
  time.sleep(3)
  small_blind_screen()


def loop():
  global logo_r_image, big_number_text, page_title, pushtext_bg, push_text, down_arrow, small_blind_active, elapsed_mins, big_blind_active, mins_label, elapsed_secs, secs_label, active_small_blind_label, active_big_blind_label, Btn46, rotary, sound_notes, current_screen, sound_notes_list, current_round, round_secs_remaining, sound_round, rotary_increment, minutes_between_rounds, small_blind_value, last_timestamp
  M5.update()
  if current_screen==1:
    if rotary.get_rotary_status():
      # Seems you can only call rotary.get_increments() once and then
      # it resets to 0. So save the value since I need to use it twice.
      rotary_increment = rotary.get_rotary_increments()
      small_blind_value = (small_blind_value if isinstance(small_blind_value, (int, float)) else 0) + 25 * rotary_increment
      if small_blind_value==225:
        small_blind_value = 200
        Speaker.tone(sound_notes['G#6'], 400)
      elif small_blind_value==0:
        small_blind_value = 25
        Speaker.tone(sound_notes['G#6'], 400)
      else:
        big_number_text.setText(str(small_blind_value))
        if rotary_increment == 1:
          Speaker.tone(sound_notes['A7'], 150)
        else:
          Speaker.tone(sound_notes['F7'], 150)
  elif current_screen==2:
    if rotary.get_rotary_status():
      # Seems you can only call rotary.get_increments() once and then
      # it resets to 0. So save the value since I need to use it twice.
      rotary_increment = rotary.get_rotary_increments()
      minutes_between_rounds = (minutes_between_rounds if isinstance(minutes_between_rounds, (int, float)) else 0) + 5 * rotary_increment
      if minutes_between_rounds==50:
        minutes_between_rounds = 45
        Speaker.tone(sound_notes['G#6'], 400)
      elif minutes_between_rounds==0:
        minutes_between_rounds = 5
        Speaker.tone(sound_notes['G#6'], 400)
      else:
        big_number_text.setText(str(minutes_between_rounds))
        if rotary_increment == 1:
          Speaker.tone(sound_notes['A7'], 150)
        else:
          Speaker.tone(sound_notes['F7'], 150)
  elif current_screen==3:
    elapsed_mins.setText(str(math.floor(round_secs_remaining / 60)))
    elapsed_secs.setText(str(round_secs_remaining % 60))
    if (time.ticks_diff((time.ticks_ms()), last_timestamp)) >= 1000:
      round_secs_remaining = (round_secs_remaining if isinstance(round_secs_remaining, (int, float)) else 0) + -1
      check_round_end()
      last_timestamp = time.ticks_ms()
  else:
    pass


if __name__ == '__main__':
  try:
    setup()
    while True:
      loop()
  except (Exception, KeyboardInterrupt) as e:
    try:
      from utility import print_error_msg
      print_error_msg(e)
    except ImportError:
      print("please update to latest firmware")
