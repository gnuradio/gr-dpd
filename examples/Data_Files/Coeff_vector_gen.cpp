#include <bits/stdc++.h>
#include<armadillo> 
using namespace std; 
#define ll long long
#define ld long double
int main()
{
	ifstream a,b;
	ofstream c;
	ld x, y;
	a.open("pa_coeff_img.txt", ios::in);
	b.open("pa_coeff_real.txt", ios::in);
	c.open("pa_coeff.txt", ios::app);
	c << "( ";
	for(ll i=0;i<28;i++)
	{
		a >> x;
		b >> y;
		c << y;
		if(x>=0)
			c << " + ";
		c << x << "j, ";
	}
	c << ")";
	a.close();
	b.close();
	c.close();
	a.open("dpd_img.txt", ios::in);
	b.open("dpd_real.txt", ios::in);
	c.open("dpd_coeff.txt", ios::app);
	c << "( ";
	for(ll i=0;i<28;i++)
	{
		a >> x;
		b >> y;
		c << y;
		if(x>=0)
			c << " + ";
		c << x << "j, ";
	}
	c << ")";
	a.close();
	b.close();
	c.close();
}