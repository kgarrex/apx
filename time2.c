
static int w_0(void *f, char buf[]){ return 0; }
static int w_1(void *f, char buf[]){ return 0; }
static int w_2(void *f, char buf[]){ return 0; }
static int w_3(void *f, char buf[]){ return 0; }
static int w_4(void *f, char buf[]){ return 0; }
static int w_5(void *f, char buf[]){ return 0; }
static int w_6(void *f, char buf[]){ return 0; }
static int w_7(void *f, char buf[]){ return 0; }
static int w_8(void *f, char buf[]){ return 0; }
static int w_9(void *f, char buf[]){ return 0; }

static int w_A(void *f, void *b){return 0;}
static int w_B(void *f, void *b){return 0;}
static int w_C(void *f, void *b){return 0;}

// The three letter day of the week
static int w_D(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	char *ptr = buf;
	const char *ptr2 = dow[f->dow].text;
	*ptr++ = *ptr2++;
	*ptr++ = *ptr2++;
	*ptr++ = *ptr2++;
	return 3;
}

static int w_E(void *f, void *b){return 0;}

// The full textual representation of a month
static int w_F(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	char *ptr = buf;
	const char length = month[f->month].length;
	const char *text = month[f->month].text;

	for(char i = 0; i < length; i++)
	{
		*ptr++ = *text++;
	}

	return length;

}

static int w_G(void *f, void *b){return 0;}

// The 24 hour format of an hour (00 - 23)
static int w_H(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	itoa_zt(f->hour, buf, 2, 10);
	return 2;
}

static int w_I(void *f, void *b){return 0;}
static int w_J(void *f, void *b){return 0;}
static int w_K(void *f, void *b){return 0;}
static int w_L(void *f, void *b){return 0;}

// The three letter month abbr 
static int w_M(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	char *ptr = buf;
	const char *ptr2 = month[f->month].text;
	*ptr++ = *ptr2++;
	*ptr++ = *ptr2++;
	*ptr++ = *ptr2++;
	return 3;
}
static int w_N(void *f, void *b){return 0;}
static int w_O(void *f, void *b){return 0;}
static int w_P(void *f, void *b){return 0;}
static int w_Q(void *f, void *b){return 0;}
static int w_R(void *f, void *b){return 0;}
static int w_S(void *f, void *b){return 0;}
static int w_T(void *f, void *b){return 0;}

//requirest a maximum of 6 bytes of space
// The formatted utc offset in hours and minutes (-00:00)
static int w_U(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	int hour, min;
	int n;
	char *ptr = buf;
	short offset = f->tz;

	/*
	if(offset == 0){
		*ptr++ = 'Z';
		return 1;
	}
	*/

	if(offset < 0) *ptr++ = '-'; 
	else *ptr++ = '+';

	offset = abs(offset);
	hour = offset/60;
	min  = offset%60;

	n = itoa_zt(hour, ptr, 2, 10);
	ptr+=n;
	*ptr++ = ':';
	n = itoa_zt(min, ptr, 2, 10);
	ptr+=n;

	return ptr - buf;
}

static int w_V(void *f, void *b){return 0;}
static int w_W(void *f, void *b){return 0;}
static int w_X(void *f, void *b){return 0;}

// The four-digit representation of a year
static int w_Y(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	int n;
	itoa_zt(f->year+1582, buf, 0, 10);
	return 4;
}

static int w_Z(void *f, void *b){return 0;}
static int w_a(void *f, void *b){return 0;}
static int w_b(void *f, void *b){return 0;}
static int w_c(void *f, void *b){return 0;}

// The two-digit day of the month (01 - 31)
static int w_d(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	//char *ptr = buf;
	itoa_zt(f->day, buf, 2, 10);
	return 2; 
}

static int w_e(void *f, void *b){return 0;}
static int w_f(void *f, void *b){return 0;}
static int w_g(void *f, void *b){return 0;}

// The twelve hour format of an hour (01 - 12)
static int w_h(token_t *tok, char buf[])
{
	return 2;
}

// The 2 digit minute with leading zeroes
static int w_i(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	itoa_zt(f->min, buf, 2, 10);
	return 2;
}

static int w_j(void *f, void *b){return 0;}
static int w_k(void *f, void *b){return 0;}

// The full textual representation of a weekday
static int w_l(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	char *ptr = buf;
	const char length = dow[f->dow].length;
	const char *text = dow[f->dow].text;

	for(char i = 0; i < length; i++)
	{
		*ptr++ = *text++;
	}


	return length;
}

// The two digit numeric month
static int w_m(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	int n;
	itoa_zt(f->month+1, buf, 2, 10);
	return 2;
}

static int w_n(void *f, void *b){return 0;}
static int w_o(void *f, void *b){return 0;}
static int w_p(void *f, void *b){return 0;}
static int w_q(void *f, void *b){return 0;}
static int w_r(void *f, void *b){return 0;}

// The 2 digit second with leading zeroes
static int w_s(token_t *tok, char buf[])
{
	apx_datetime f = tok->user_data;
	itoa_zt(f->sec, buf, 2, 10);
	return 2;
}

static int w_t(void *f, void *b){return 0;}
static int w_u(void *f, void *b){return 0;}
static int w_v(void *f, void *b){return 0;}
static int w_w(void *f, void *b){return 0;}
static int w_x(void *f, void *b){return 0;}
static int w_y(void *f, void *b){return 0;}
static int w_z(void *f, void *b){return 0;}


static int r_0(void *f, void *b){return 0;}
static int r_1(void *f, void *b){return 0;}
static int r_2(void *f, void *b){return 0;}
static int r_3(void *f, void *b){return 0;}
static int r_4(void *f, void *b){return 0;}
static int r_5(void *f, void *b){return 0;}
static int r_6(void *f, void *b){return 0;}
static int r_7(void *f, void *b){return 0;}
static int r_8(void *f, void *b){return 0;}
static int r_9(void *f, void *b){return 0;}
static int r_A(void *f, void *b){return 0;}
static int r_B(void *f, void *b){return 0;}
static int r_C(void *f, void *b){return 0;}
static int r_D(void *f, void *b){return 0;}
static int r_E(void *f, void *b){return 0;}
static int r_F(void *f, void *b){return 0;}
static int r_G(void *f, void *b){return 0;}

// Read the 2 digit hour
static int r_H(token_t *tok, const char *str){
	apx_datetime f = tok->user_data;
	unsigned n;

	n = atouint(str, 2);

	f->hour = n;
	return 2;
}
static int r_I(void *f, void *b){return 0;}
static int r_J(void *f, void *b){return 0;}
static int r_K(void *f, void *b){return 0;}
static int r_L(void *f, void *b){return 0;}
static int r_M(void *f, void *b){return 0;}
static int r_N(void *f, void *b){return 0;}
static int r_O(void *f, void *b){return 0;}
static int r_P(void *f, void *b){return 0;}
static int r_Q(void *f, void *b){return 0;}
static int r_R(void *f, void *b){return 0;}
static int r_S(void *f, void *b){return 0;}
static int r_T(void *f, void *b){return 0;}

//Read the iso8601 time zone
static int r_U(token_t *tok, const char *str){
	apx_datetime f = tok->user_data;
	int n;
	int tz;
	char neg = 0;

	if(*str == 'Z'){
		f->tz = 63;
		return 1;	
	}

	if(*str == '+') str++;
	else if(*str == '-'){
		neg = 1;
		*str++;
	}
	else return 0;

	n = atouint(str, 2);
	tz = n * 60;

	str += 2;

	if(*str != ':') return 0;
	str++;

	n = atouint(str, 2);
	tz += n;

	tz = neg ? -tz : tz;

	f->tz = abs((tz/15)-63);

	return 6;
}

static int r_V(void *f, void *b){return 0;}
static int r_W(void *f, void *b){return 0;}
static int r_X(void *f, void *b){return 0;}

// Read the 4 digit year
static int r_Y(token_t *tok, const char *str)
{
	apx_datetime f = tok->user_data;
	unsigned n;
	for(int i = 0; i < 4; i++){
		if(str[i] < '0' || str[i] > '9') return 0;
	}
	n = atouint(str, 4);

	if(n < 1582 || n > 9774) return 0;
	f->year = n - 1582;
	return 4;
}

static int r_Z(void *f, void *b){return 0;}
static int r_a(void *f, void *b){return 0;}
static int r_b(void *f, void *b){return 0;}
static int r_c(void *f, void *b){return 0;}

// Read the 2 digit day
static int r_d(token_t *tok, const char *str){
	apx_datetime f = tok->user_data;
	unsigned n;

	n = atouint(str, 2);

	f->day = n;
	return 2;
}
static int r_e(void *f, void *b){return 0;}
static int r_f(void *f, void *b){return 0;}
static int r_g(void *f, void *b){return 0;}
static int r_h(void *f, void *b){return 0;}

// Read the 2 digit minute 
static int r_i(token_t *tok, const char *str){
	apx_datetime f = tok->user_data;
	unsigned n;

	n = atouint(str, 2);

	f->min = n;
	return 2;
}

static int r_j(void *f, void *b){return 0;}
static int r_k(void *f, void *b){return 0;}
static int r_l(void *f, void *b){return 0;}

// Read the 2 digit month 
static int r_m(token_t *tok, const char *str){
	apx_datetime f = tok->user_data;
	unsigned n;

	//TODO str stopping early
	n = atouint(str, 2);

	if(n < 1 || n > 12) return 0;
	f->month = n-1;
	return 2;
}

static int r_n(void *f, void *b){return 0;}
static int r_o(void *f, void *b){return 0;}
static int r_p(void *f, void *b){return 0;}
static int r_q(void *f, void *b){return 0;}
static int r_r(void *f, void *b){return 0;}

// Read the 2 digit second
static int r_s(token_t *tok, const char *str){
	apx_datetime f = tok->user_data;
	unsigned n;

	n = atouint(str, 2);

	f->sec = n;
	return 2;
}

static int r_t(void *f, void *b){return 0;}
static int r_u(void *f, void *b){return 0;}
static int r_v(void *f, void *b){return 0;}
static int r_w(void *f, void *b){return 0;}
static int r_x(void *f, void *b){return 0;}
static int r_y(void *f, void *b){return 0;}
static int r_z(void *f, void *b){return 0;}


