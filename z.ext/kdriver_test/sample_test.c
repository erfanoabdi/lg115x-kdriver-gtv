#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#include "sample_kapi.h"
#include "simple_lib.h"

static int			sample_fd;

/*----------------------------------------------------------------------------------------
 * TIMER TEST
 *----------------------------------------------------------------------------------------*/
void	timer_test ( void )
{
	int	ret;

	UINT32	tmval;

	tmval = ReadDecimalKeyInput("Enter timeout (msec) > ");

	ret = ioctl( sample_fd, SAMPLE_IOW_TEST_TIMEOUT, &tmval );

	MsecSleep( tmval );

	tmval = ReadDecimalKeyInput("Enter timetick (msec) > ");

	ret = ioctl( sample_fd, SAMPLE_IOW_TEST_TIMETICK, &tmval );

	MsecSleep( tmval*10 );

	printf("<!> timer test done\n");
}



/*----------------------------------------------------------------------------------------
 * EVENT TEST
 *----------------------------------------------------------------------------------------*/
static pthread_t 	EventReceiverTaskHandle;
static LX_SAMPLE_TEST_EVENT_READER_T	revent;
static LX_SAMPLE_TEST_EVENT_WRITER_T	wevent;
static BOOLEAN						event_test_done = FALSE;

void* EventReceiverTask(void *data)
{
	int				ret;

	printf("<!> recv evnt begin (evnt:%08X, opt:%d, tm:%d\n", revent.ev, revent.option, revent.timeout );

	ret = ioctl( sample_fd, SAMPLE_IORW_TEST_RECV_EVENT, &revent );

	if ( !ret )
	{
		printf("<!> recv evnt error. ret = %d\n", ret );
	}
	else
	{
		printf("<!> recv evnt END = %08X\n", revent.rev );
	}

	event_test_done = TRUE;

	return (void*)1;
}

void	event_test	(void)
{
	UINT32			data;
	int				sleep_time;
	int				ret;
	int				status;

	memset( &revent, 0x0, sizeof(LX_SAMPLE_TEST_EVENT_READER_T));
	memset( &wevent, 0x0, sizeof(LX_SAMPLE_TEST_EVENT_WRITER_T));

	/* read basic reader/writer parameter */
	printf( "Enter value for event receiver...\n");
	revent.ev 		= ReadHexKeyInput(     "event value (hex)     > ");
	revent.timeout 	= ReadDecimalKeyInput( "timeout     (msec)    > ");
	revent.option	= ReadHexKeyInput(     "option (0:ANY, 1:ALL) > ");

	printf( "Enter value for event writer...\n");
	wevent.ev 		= ReadHexKeyInput(     "event value (hex)     > ");
	sleep_time 		= ReadDecimalKeyInput( "issue time  (msec)    > ");

	event_test_done = FALSE;

	ret = ioctl( sample_fd, SAMPLE_IO_TEST_CLR_EVENT, NULL );

	if (pthread_create(&EventReceiverTaskHandle, NULL, EventReceiverTask, NULL ) < 0)
	{
		perror("thread create error:");
		exit(0);
	}

	while(1)
	{
		MsecSleep(sleep_time);
		ret = ioctl( sample_fd, SAMPLE_IOW_TEST_SEND_EVENT, &wevent );

		MsecSleep(100);

		if ( event_test_done ) break;

		printf( "Enter value for event writer...\n");
		wevent.ev 		= ReadHexKeyInput(     "event value (hex)     > ");
		sleep_time 		= ReadDecimalKeyInput( "issue time  (msec)    > ");
	}

	/* wait until thread terminate */
    pthread_join(EventReceiverTaskHandle, (void **)&status);

	printf("<!> event test done\n");
}

/*----------------------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------
 * MAIN
 *----------------------------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{
	int				ret;
	CHIP_REV_INFO_T	rev_info;
	UINT32			u32_data = 0x1234;

	sample_fd = open( "/dev/lg/sample0", O_RDONLY|O_RDWR );

	ret = ioctl( sample_fd, SAMPLE_IO_RESET, NULL );
	ret = ioctl( sample_fd, SAMPLE_IOR_CHIP_REV_INFO, &rev_info );
	ret = ioctl( sample_fd, SAMPLE_IOW_WRITE_UINT32, &u32_data );

	timer_test ( );

	event_test ( );

 	close(sample_fd);

	return 0;
}

