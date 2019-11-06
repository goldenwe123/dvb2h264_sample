#include"ts_packet.h"
#include"gn_dvb.h"
#include <linux/dvb/dmx.h>
#include <linux/dvb/version.h>
#include <linux/dvb/frontend.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(){

char frontend_devname[]="/dev/dvb/adapter0/frontend0";
char demux_devname[]="/dev/dvb/adapter0/demux0";
char *chanfile = "channels.conf";
FILE *channel_file = fopen(chanfile, "r");
struct gn_program *program;
struct gn_dvb *device;
int channel = 11;
int ret;


int nums = dvbcfg_zapchannel_parse(channel_file,&program);

printf("######\n");

FILE *pFile,*pFile2;
pFile2 = fopen( "demo.ts","w" );
pFile = fopen( "demo.h264","w" );

char buf[1880];

int count=0;
//Ts_packet *packet;

while(1) {

	ret = dvb_open_device(frontend_devname,demux_devname,device);

	if(ret) {
        	printf("open device faild \n");
        	return 1;
	}

	ret = dvb_open_chennel(device,&program[channel]);

	if(ret) {
        	printf("open channel faild \n");
        	return 1;
	}

	for(int i=0;i<32000;i++) {
		char buf[188]={0};
		int num=read(device->demux_fd,buf,188);
		///if(num<0)
		printf("***num %d %x\n",num,buf[0]);
		if(num>0) {

			//fwrite(&buf,1,num,pFile2);
			Ts_packet *packet;
			int ret=ts_packet_init(buf,&packet);
			if(ret>0) {
				if(packet->ts_header.pid==program[channel].video_pid) {
					int write_count=0;
					int write_p=0;

					num=fwrite(&packet->payload.data[0],1,packet->payload.size,pFile);
					free(packet);
					packet=NULL;
					//count+=num;
					//printf("count: %d\n",count);
				}
        	        	//printf("count: %d\n",count);
			}
			//count+=num;
			//printf("count: %d\n",count);
		}
	}

	ret=dvb_close_device(device);

	if(ret) {
                printf("cloae device faild \n");
                return 1;
        }

	printf("change channel %d\n",channel);
	usleep(500000);
	channel++;
	if(channel==13)
			channel=11;
}





return 0;
}
