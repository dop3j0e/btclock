/*
 * config.c
 *
 *  Created on: 13.02.2013
 *      Author: dojoe
 */

#include "btclock.h"
#include <avr/eeprom.h>

EEMEM struct config_s config = { .sequence = {{ SEQ_TIME, 1}}};
struct sequence_entry sequence[MAX_SEQUENCE];
uint8_t countdown;
uint8_t sequence_ptr;
uint8_t cur_line = SEQ_NOTHING;

static void set_display_line(uint8_t *src, uint8_t is_eeprom, uint8_t mode)
{
	char *dest = text_line + 3;
	uint8_t c, len = 0;

	display_mode = STATIC;

	memset(text_line, 0, sizeof(text_line));
	while (1)
	{
		c = is_eeprom ? eeprom_read_byte(src++) : *src++;
		if (!c)
			break;
		if ('.' == c && len)
			*(dest - 1) |= 0x80;
		else
		{
			*dest++ = c;
			len++;
		}
	}

	text_line_offset = 0;
	text_line_length = len;
	display_mode = mode ? TEXT_2 : TEXT_1;
}

static void set_display(uint8_t which)
{
	if (cur_line == which)
		return;

	if (SEQ_TIME == which)
	{
		display_mode = TIME;
		tlc_show_time();
	}
	else if (SEQ_DATE == which)
	{
		display_mode = DATE;
		tlc_show_date();
	}
	else
	{
		set_display_line((uint8_t *)config.lines[which], 1, eeprom_read_byte(&config.line_modes) & (1 << which));
	}
	cur_line = which;
}

void next_line()
{
	uint8_t which = sequence[sequence_ptr].which;
	countdown = sequence[sequence_ptr].duration;

	set_display(which);

	sequence_ptr++;
	if (MAX_SEQUENCE == sequence_ptr || 0 == sequence[sequence_ptr].duration)
		sequence_ptr = 0;
}

static uint8_t in_timespan(struct timespan *span)
{
	uint16_t cur_time   = time.hour << 8 | time.minute;
	uint16_t span_start = eeprom_read_word(&span->start);
	uint16_t span_end   = eeprom_read_word(&span->end);
	uint8_t after_start = (cur_time >= span_start);
	uint8_t before_end  = (cur_time < span_end);
	if (span_start <= span_end)
	{
		return after_start && before_end;
	}
	else
	{
		return after_start || before_end;
	}
}

void check_timespans()
{
	uint8_t i = NUM_SPECIALS;
	blank = in_timespan(&config.blank_time) ||
			(eeprom_read_byte(&config.blank_weekend) && time.weekday >=5);
	while (i)
	{
		if (in_timespan(&config.specials[i].when))
		{
			set_display(eeprom_read_byte(&config.specials[i].what));
			countdown = 60; // so we get a normal update the next minute _if_ we're not still in the timespan
		}
		i--;
	}
}

void set_line(int8_t index, char *buf, uint8_t length, uint8_t mode)
{
	uint8_t line_modes, mode_mask;

	memset(buf + length, 0, TEXT_MAX - length);

	if (index < 0)
	{
		set_display_line((uint8_t *)buf, 0, mode);
		countdown = 42;
	}
	else
	{
		eeprom_update_block(buf, config.lines[index], TEXT_MAX);

		line_modes = eeprom_read_byte(&config.line_modes);
		mode_mask = 1 << index;
		if (mode)
			line_modes |= mode_mask;
		else
			line_modes &= ~mode_mask;
		eeprom_update_byte(&config.line_modes, line_modes);
	}

	cur_line = SEQ_NOTHING; // make sure we copy the line at next update
}

