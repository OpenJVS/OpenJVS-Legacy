void print_state(struct cwiid_state *state)
{
	float tmpx, tmpy, count, xVector, yVector = 0;
	float[2] initialX;
	float[2] initialY;

	/* Find two IR values and set them to our values in initialX and initialY */
	for (int i = 0; i < CWIID_IR_SRC_COUNT; i++)
	{
		if (state->ir_src[i].valid && count < 2)
		{
			initialX[count] = state->ir_src[i].pos[CWIID_X];
			initialY[count] = state->ir_src[i].pos[CWIID_Y];
			count++;
		}
	}

	/* If there are 2 IR LEDs visible, calculate the average between them to get the center dot */
	if(count > 1)
	{
		if(initialX[0] < initialX[1])
		{
			xVector = (initialX[1] - initialX[0]) / 2;
			yVector = (initialY[1] - initialY[0]) / 2;
		}
		else
		{
			xVector = (initialX[0] - initialX[1]) / 2;
			yVector = (initialY[0] - initialY[1]) / 2;
		}

		tmpx = (initialX[0] + initialX[1]) / 2;
		tmpy = (initialY[0] + initialY[1]) / 2;
	}
	else if(count == 1)
	{
		if(initialX[0] > 512)
		{
			tmpx = initialX[0] + xVector;
			tmpy = initialY[0] + yVector;
		}
		else
		{
			tmpx = initialX[0] - xVector;
			tmpy = initialY[0] + yVector;
		}
	}

	float n_x = (tmpx / 1024) * 255;
	float n_y = (tmpy / 1024) * 255;

	int final_x = 255 - (int) n_x;
	int final_y = (int) n_y;

	printf("%d, %d\n", final_x, final_y);
}
