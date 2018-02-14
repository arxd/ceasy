char s_heart[] = {0,1,1,4,1,1};
char s_club[] = {1,1,1,1,1,1,1,1,1};
char s_diamond[] = {1,1,1,3,1,1,1};
char s_spade[] = {1,1,2,1,1,3};
char s_zero[] = {1,1,1,1,1,2,1,2,1,1,1,1,1};
char s_one[] = {1,1,1,2,2,1,2,1,1,3};
char s_two[] = {0, 2,3,1,1,1,1,1,2,3};
char s_three[] = {0, 3,2,1,1,1,3,4};
char s_four[] = {0, 1,2,1,1,4,2,1,2,1};
char s_five[] = {0, 4,2,2,3,3,1};
char s_six[] = {0, 4,2,4,1,4};
char s_seven[] = {0, 3,2,1,1,1,2,1,2,1,1};
char s_eight[] = {0, 4,1,5,1,4};
char s_nine[] = {0, 2,1,1,1,4,2,1,2,1};
char s_ten[] = {0, 1,2,1,1,1,1,1,1,2,1,1,1,2,1,1,1,2,2,1,1};
char s_jack[] = {2,1,2,1,2,2,1,4};
char s_queen[] = {1,1,1,1,1,2,1,1,1,1,3,1};
char s_king[] = {0,1,2,1,2,1,1,3,1,1,1,1};
char s_ace[] = {1,1,1,1,1,2,1,5,1,1};

char m_heart[] = {1,1,1,1,1,10,1,3,3,1,2};
char m_club[] = {1,3,2,3,1,1,1,1,1,6,2,1,2};
char m_diamond[] = {2,1,3,3,1,5,1,3,3,1,2};
char m_spade[] = {2,1,3,3,1,10,2,1,2};
char m_zero[] = {1,3,1,1,3,2,3,2,3,2,3,2,3,2,3,1,1,3,1};
char m_one[] = {2,1,3,2,2,1,1,1,4,1,4,1,4,1,4,1,2,5};
char m_two[] = {1,3,1,1,3,1,4,1,3,1,3,1,3,1,3,1,4,5};
char m_three[] = {1,3,1,1,3,1,4,1,2,2,5,1,4,2,3,1,1,3,1};
char m_four[] = {3,1,3,2,3,2,2,1,1,1,2,1,1,1,1,1,2,1,1,5,3,1,1};
char m_five[] = {0,6,4,1,5,3,5,2,3,2,3,1,1,3,1};
char m_six[] = {1,3,1,1,3,2,4,4,1,1,3,2,3,2,3,1,1,3,1};
char m_seven[] = {0,5,4,1,3,1,3,1,4,1,3,1,4,1,4,1,3};
char m_eight[] = {1,3,1,1,3,2,3,1,1,3,1,1,3,2,3,2,3,1,1,3,1};
char m_nine[] = {1,3,1,1,3,2,3,2,3,1,1,4,4,2,3,1,1,3,1};
char m_ten[] = {3,1,1,1,1,1,1,2,1,1,1,2,1,1,1,2,1,1,1,2,1,1,1,2,1,1,1,2,2,1,1};
char m_jack[] = {2,3,3,1,4,1,4,1,4,1,1,1,2,1,1,1,2,1,2,2,2};
char m_queen[] = {1,2,2,1,2,1,1,1,2,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,2,1,2,2,1,1};
char m_king[] = {0,1,3,2,2,1,1,1,1,1,2,3,2,1,2,1,1,1,2,1,1,1,3,2,3,1};
char m_ace[] = {2,1,4,1,3,1,1,1,2,1,1,1,2,1,1,1,2,3,1,1,3,2,3,1};

char b_heart[] = {1,2,5,2,1,4,3,9,1,27,1,9,2,9,3,7,5,5,7,3,9,1,5};
char b_club[] = {4,3,7,5,6,5,6,5,4,2,1,3,1,2,1,4,1,1,1,19,1,1,1,4,1,2,2,1,2,2,5,3,7,5,3};
char b_spade[] = {5,1,9,3,7,5,5,7,3,9,1,33,1,3,1,1,1,3,5,3,7,5,3};
char b_diamond[] = {5,1,9,3,7,5,5,7,3,9,1,11,1,9,3,7,5,5,7,3,9,1,5};
char b_zero[] = {3,4,4,8,1,3,4,5,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,5,4,3,1,8,4,4,3};
char b_one[] = {4,2,7,3,6,4,5,5,5,2,1,2,8,2,8,2,8,2,8,2,8,2,8,2,8,2,8,2,5,8,2,8,1};
char b_two[] = {3,4,4,8,1,3,4,5,6,4,6,2,8,2,7,3,5,4,4,4,5,3,6,3,6,3,7,2,6,22};
char b_three[] = {0, 22,5,2,7,2,7,2,7,2,7,4,6,6,8,3,8,2,8,4,6,5,4,3,1,8,4,4,3};
char b_four[] = {6,2,7,3,6,4,6,4,5,5,5,2,1,2,4,3,1,2,4,2,2,2,3,3,2,2,3,2,3,2,2,20,6,2,7,4,6,4,1};
char b_five[] = {0, 9,1,9,1,2,8,2,8,2,8,2,1,4,3,9,1,3,4,5,6,2,8,2,8,4,6,5,4,3,1,8,4,4,3};
char b_six[] = {3,4,4,8,1,3,4,5,6,4,8,8,2,9,1,3,4,5,6,4,6,4,6,4,6,5,4,3,1,8,4,4,3};
char b_seven[] = {0,22,6,2,7,3,6,3,6,3,7,2,7,2,8,2,7,2,8,2,7,2,8,2,8,2,8,2,6};
char b_eight[] = {3,4,5,6,3,3,2,3,2,2,4,2,2,2,4,2,3,2,2,2,4,6,3,8,1,3,4,5,6,4,6,4,6,5,4,3,1,8,4,4,3};
char b_nine[] = {3,4,4,8,1,3,4,5,6,4,6,4,6,4,6,5,4,3,1,9,2,8,8,4,6,5,4,3,1,8,4,4,3}; 
char b_ten[] = {6,2,2,2,3,4,1, 2,2,8, 2,2,2,4, 2,2,2,4, 2,2,2,4, 2,2,2,4, 2,2,2,4, 2,2,2,4, 2,2,2,4, 2,2,2,4, 2,2,2,4, 2,2,2,4, 3,4,1,2,4,2,2};
char b_jack[] = {5,4,6,4,7,2,8,2,8,2,8,2,8,2,8,2,8,2,8,2,2,2,4,2,2,2,4,2,2,3,2,3,3,6,5,4,4};
char b_queen[] = {3,4,5,6,4,2,2,2,3,2,4,2,2,2,4,2,2,2,4,2,2,2,4,2,2,2,4,2,1,4,3,2,1,6,1,2,2,2,1,5,2,2,2,4,3,2,2,2,4,8,3,4,1,2};
char b_king[] = {0,4,2,8,2,4,1,2,4,2,2,2,3,3,2,2,2,3,3,2,1,3,4,5,5,6,4,3,1,2,4,2,3,2,3,2,3,2,3,2,4,2,2,2,4,2,1,4,2,8,2,4};
char b_ace[] = {4,2,8,2,8,2,7,4,6,1,2,1,6,1,2,1,5,2,2,2,4,2,2,2,4,6,4,6,3,3,2,3,2,2,4,2,2,2,4,2,1,4,2,8,2,4};


void draw_img(int x, int y, int w, int h, char*data, int color)
{
	int i=0, hi = 0, sum=0, c = 0;
	while (i < w*h) 
	{
		if (sum+data[hi] == i) {
			c = (!c)*color;
			sum += data[hi];
			hi ++;
		}
		vram[(i/w+y)*256 + i%w+x] = c;
		i++;
	}
}

void draw_suit(int x, int y, int suit, int size)
{
	char *data[] = {
		s_spade, s_heart, s_club, s_diamond,
		m_spade, m_heart, m_club, m_diamond,
		b_spade, b_heart, b_club, b_diamond};
	int w = (int[3]){3,5,11}[size];
	draw_img(x,y,w,w,data[4*size+suit], (suit&1)+1);
}

void draw_num(int x, int y, int num, int color, int size)
{
	char *data[] = {
		s_zero, s_one, s_two, s_three, s_four, s_five, s_six, s_seven, s_eight, s_nine, s_ten, s_jack, s_queen, s_king, s_ace,
		m_zero, m_one, m_two, m_three, m_four, m_five, m_six, m_seven, m_eight, m_nine, m_ten, m_jack, m_queen, m_king, m_ace,
		b_zero, b_one, b_two, b_three, b_four, b_five, b_six, b_seven, b_eight, b_nine, b_ten, b_jack, b_queen, b_king, b_ace};
	int w = (int[3]){3,5,10}[size];
	int h = (int[3]){5,8,15}[size];
	if (size == 0 && num == 10) {
		x -= 2;
		w += 2;
	}
	draw_img(x, y, w, h,data[15*size+num], color);
}