#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>

#include <stdio.h>
#include <stdlib.h> // For exit() function
#include "PanTompkins.h"
#include "PanTompkins.c"
#define LOOP_TIMEOUT_MS 50 // 10 seconds
#define BUFFSIZE 100 // Assuming the size of inputSignal and outputSignal is fixed


void my_expiry_function(struct k_timer *timer_id);
void my_timer_stop_function(struct k_timer *timer_id);

K_TIMER_DEFINE(my_timer, my_expiry_function, my_timer_stop_function);

// Global flag to indicate if the loop should continue or not
bool run_loop = true;
void delay_ms(int ms) {
    k_msleep(ms);
}
void my_expiry_function(struct k_timer *timer_id) {
    printk("Timer expired\n");
    run_loop = false; // Set the flag to stop the loop
}

void my_timer_stop_function(struct k_timer *timer_id) {
    printk("Timer stopped\n");
}

int main() {
	
	// --------------Input Arguments ------------------ //
	k_timer_start(&my_timer, K_MSEC(LOOP_TIMEOUT_MS), K_NO_WAIT); // Start the timer

    uint32_t start_time = k_uptime_get_32(); // Get the current time
    while (run_loop) {
        // Perform your actions within the loop here
        
        // Check if the elapsed time exceeds the timeout
        if (k_uptime_get_32() - start_time >= LOOP_TIMEOUT_MS) {
            printk("Loop timeout\n");
            break;
        }
    }

    printk("Loop completed\n");

    k_timer_stop(&my_timer); // Stop the timer

	// Input signal
	
	
	//PT_init(); // Always Initialize the Algorithm before use ---> This prepares all filters and parameters
	
	int16_t inputSignal[BUFFSIZE] = {942, 940, 941, 942, 941, 938, 937, 937, 937, 937, 938, 937, 934, 932, 934, 937, 936, 935, 933, 933, 934, 934, 935, 935, 930, 931, 933, 935, 934, 935, 932, 930, 926, 920, 916, 911, 902, 896, 896, 911, 935, 961, 992, 1036, 1092, 1144, 1181, 1194, 1172, 1115, 1040, 972, 927, 910, 908, 913, 923, 925, 929, 927, 927, 926, 927, 930, 929, 926, 925, 924, 923, 926, 925, 924, 925, 925, 925, 929, 929, 926, 925, 926, 928, 928, 928, 926, 924, 924, 928, 929, 927, 928, 923, 927, 928, 931, 928, 926, 925, 926, 927};
	
	//{926, 920, 916, 911, 902, 896, 896, 911, 935, 961, 992, 1036, 1092, 1144, 1181, 1194, 1172, 1115, 1040, 972, 942, 940, 941, 942, 941, 938, 937, 937, 937, 937, 938, 937, 934, 932, 934, 937, 936, 935, 933, 933, 934, 934, 935, 935, 930, 931, 933, 935, 934, 935 };
	// Output signal
	int16_t outputSignal[BUFFSIZE] = {0};
	
	int16_t delay, Rcount, s1, s2, s3, s4, s5, ThF1;
	uint16_t ThI1, SPKI, NPKI;
	int32_t RLoc, SampleCount;
	SampleCount = 0;

	Rcount = 0;
	
	// ------ Pass the signal sample by sample mimicking a real-time scenario ----------- //
	for (int i = 0; i < BUFFSIZE; ++i) {
		++SampleCount;

		delay = PT_StateMachine((int16_t) inputSignal[i]); // This is the main function of the algorithm

		// ------- A positive delay to current sample is returned in case of beat detection ----------- //
		if (delay != 0) {
			RLoc = SampleCount - (int32_t) delay;
			++Rcount;
		}
		else {
			RLoc = 0;
		}

		// -------- Toolbox comes with many helper functions for debugging, see PanTompkins.c for more details ---------- //
		s1 = PT_get_LPFilter_output();
		s2 = PT_get_HPFilter_output();
		s3 = PT_get_DRFilter_output();
		s4 = PT_get_SQRFilter_output();
		s5 = PT_get_MVFilter_output();

		ThI1 = PT_get_ThI1_output();
		SPKI = PT_get_SKPI_output();
		NPKI = PT_get_NPKI_output();
		ThF1 = PT_get_ThF1_output();

		outputSignal[i] = (float)RLoc; // Store RLoc in the output signal array
		
		// Uncomment the lines below if you want to print the results to console
		
		printf("Sample: %d, RLoc: %d, s1: %d, s2: %d, s3: %d, s4: %d, s5: %d, ThI1: %d, SPKI: %d, NPKI: %d, ThF1: %d\n",
			inputSignal[i], RLoc, s1, s2, s3, s4, s5, ThI1, SPKI, NPKI, ThF1);
		
	
	
	return 0;
}
