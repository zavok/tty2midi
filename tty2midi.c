#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<termio.h>
#include<alsa/asoundlib.h>

int
main(void)
{
	int tf, portid;
	struct termios tty;
	snd_seq_t *seq;
	snd_seq_event_t ev;
	snd_midi_event_t *midi;

	/* serial setup */
	tf = open("/dev/ttyACM0", O_RDWR);
	if (tf < 0) {
		fprintf(stderr, "can't open\n");
		exit(-1);
	}
	if (tcgetattr(tf, &tty) != 0) {
		fprintf(stderr, "tcgetattr failed\n");
		exit(-1);
	}
	if (cfsetspeed(&tty, B115200) != 0) {
		fprintf(stderr, "cfsetspeed failed\n");
		exit(-1);
	}
	tty.c_cflag &= ~(PARENB|CSTOPB|CSIZE|CRTSCTS);
	tty.c_cflag |= CS8|CREAD|CLOCAL;
	tty.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHONL|ISIG);
	tty.c_iflag &= ~(IXON |IXOFF| IXANY|IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	tty.c_oflag &= ~(OPOST|ONLCR);
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 1;
	tcsetattr(tf, TCSANOW, &tty);
	
	/* ALSA MIDI setup */
	if (snd_seq_open(&seq, "default", SND_SEQ_OPEN_OUTPUT, 0) != 0) exit(-1);
	if (snd_seq_set_client_name(seq, "tty2midi") != 0) {
		fprintf(stderr, "set client name failed\n");
		exit(-1);
	}
	portid = snd_seq_create_simple_port(
	  seq,
	  "tty2midi out",
	  SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
	  SND_SEQ_PORT_TYPE_APPLICATION
	);
	if (portid < 0) {
		fprintf(stderr, "create simple port failed\n");
		exit(-1);
	}
	if (snd_midi_event_new(16, &midi) != 0) {
		fprintf(stderr, "midi event new failed\n");
		exit(-1);
	}
	while(1) {
		unsigned char buf[1024];
		int n, r;
		/* TODO: add poll(2) */
		if ((n = read(tf, buf, 1)) == 0) continue;

		//fprintf(stderr, "%x %x %x\t", buf[0],
		//  buf[1], buf[2]);
		
		snd_seq_ev_clear(&ev);
		snd_seq_ev_set_subs(&ev);
		snd_seq_ev_set_direct(&ev);
		snd_seq_ev_set_source(&ev, portid);
		
		r = snd_midi_event_encode_byte(midi, *buf, &ev);
		if (r < 0) {
			fprintf(stderr, "event encode failed\n");
		}
		if (r != 1) continue;

		r = snd_seq_event_output_direct(seq, &ev);
		if (r < 0) {
			fprintf(stderr, "event output failed: %s\n", snd_strerror(r));
		}
	}
	return 0;
}
