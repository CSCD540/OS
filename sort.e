x,y, arry[3], q[6], ar0,ar1,ar2, ar7, c0, c1;

process p0
{ 
	if(x == 8) 
	{
		halt;
	}
}

process p1
{
	q[0] = 3;
	q[1] = 9; 
	q[2] = 1; 
	q[3] = 5; 
	q[4] = 7; 
	q[5] = 5;

	r0 = 1; 
	r1 = 0; 
	r2 = 0; 
	while( r0 == 1 ) 
	{ 
		r0 = 0; 
		ar0 = r0; 
		r1 = 0; 
		ar1 = r1;
		 
		while( r1 < 5) 
		{ 
			ar1 = r1; 
			r7 = r1 + 1; 
			ar7 = r7; 
			c0 = q[r1]; 
			c1 = q[r7];
			 
			if( q[r1] > q[r7] ) 
			{ 
				r3 = q[r1]; 
				q[r1] = q[r7]; 
				q[r7] = r3; 
				r0 = 1; 
				ar0 = r0; 
			} 
			r1 = r1 + 1; 
		} 
		ar2 = r2; 
	} 
	x = 8;
}

