#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
#include<GL/glut.h>

//for time
#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
//for time
double cameraAngle;
double move_X, move_Y, move_Z;
int canDrawGrid, canDrawAxis;
double speed=30,ang_speed=.1;
double cameraRadius, cameraHeight, cameraAngleDelta;
int num_texture  = -1;
int random_num =-1;
int pos,neg;
GLuint field;
GLuint net;
GLuint football;
GLuint stadium;

/************************************MYDECLARATION*************************/
double point_X;
int go,goo, stop;
int correctSot;
int dis;
int p;
double point_x, point_z;
double Kep_x , Kep_y , Kep_z;
double ball_x, ball_y, ball_z;
int direction;
double cc_xx,cc_yy,cc_zz;
double shotSpeed;
double Kep_rx,Kep_ry,Kep_rz,Kep_ang;

double initBall_xx, initBall_yy, initBall_zz, secBall_xx, secBall_yy, secBall_zz, thiBall_xx, thiBall_yy, thiBall_zz;
double initKep_xx, initKep_yy, initKep_zz, secKep_xx, secKep_yy, secKep_zz, thiKep_xx, thiKep_yy, thiKep_zz;


/***************************** VECTOR structure **********************************/

struct V;

V operator+(V a,V b);
V operator*(V a,V b);
V operator*(V b,double a);
V operator*(double a,V b);

struct V{
	double x,y,z;

	V(){}
	V(double _x,double _y){x=_x;y=_y;z=0;}
	V(double _x,double _y,double _z){x=_x;y=_y;z=_z;}

	
	double	mag(){	return sqrt(x*x+y*y+z*z);	}
	
	void 	norm(){	double d = mag();x/=d;y/=d;	z/=d;}
	V 		unit(){	V ret = *this;	ret.norm(); return ret;}

	double	dot(V b){		return x*b.x + y*b.y + z*b.z;}
	V		cross(V b){		return V( y*b.z - z*b.y , z*b.x - x*b.z , x*b.y - y*b.x );}
	

	double	projL(V on){	on.norm();	return this->dot(on);}
	V		projV(V on){	on.norm();	return on * projL(on);}

	V rot(V axis, double angle){
		return this->rot(axis, cos(angle), sin(angle));
	}
	
	V rot(V axis, double ca, double sa){
		V rotatee = *this;
		axis.norm();
		V normal = (axis * rotatee).unit();
		V mid = (normal * axis).unit();
		double r = rotatee.projL(mid);
		V ret=r*mid*ca + r*normal*sa + rotatee.projV(axis);
		return ret.unit();
	}
};

V operator+(V a,V b){		return V(a.x+b.x, a.y+b.y, a.z+b.z);	}
V operator-(V a){			return V (-a.x, -a.y, -a.z);			}
V operator-(V a,V b){		return V(a.x-b.x, a.y-b.y, a.z-b.z);	}
V operator*(V a,V b){		return a.cross(b);						}
V operator*(double a,V b){	return V(a*b.x, a*b.y, a*b.z);			}
V operator*(V b,double a){	return V(a*b.x, a*b.y, a*b.z);			}
V operator/(V b,double a){	return V(b.x/a, b.y/a, b.z/a);			}




V loc,dir,perp;


V  _L(0,-150,20);
V  _D(0,1,0);
V  _P(0,0,1);


/***************************** Texture Functions *******************************/



int LoadBitmapImage(char *filename)
{
    int i, j=0;
    FILE *l_file;
    unsigned char *l_texture;

    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;

    num_texture++;

    if( (l_file = fopen(filename, "rb"))==NULL) return (-1);

    fread(&fileheader, sizeof(fileheader), 1, l_file);

    fseek(l_file, sizeof(fileheader), SEEK_SET);
    fread(&infoheader, sizeof(infoheader), 1, l_file);

    l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
    memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
	for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
		{
				fread(&rgb, sizeof(rgb), 1, l_file);

				l_texture[j+0] = rgb.rgbtRed;
				l_texture[j+1] = rgb.rgbtGreen;
				l_texture[j+2] = rgb.rgbtBlue;
				l_texture[j+3] = 255;
				j += 4;
		}
    fclose(l_file);

    glBindTexture(GL_TEXTURE_2D, num_texture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    free(l_texture);

    return (num_texture);

}

void loadImage()
{
	field = LoadBitmapImage("image/fgrass.bmp");
	net = LoadBitmapImage("image/net.bmp");
	football = LoadBitmapImage("image/football.bmp");
	stadium = LoadBitmapImage("image/stadium.bmp");
	/*if(field != -1)
		printf("Load successful!!\n");
	else printf("Image loading Failed!!\n");*/

}


void DrawCircle(float cx, float cy, float r, int num_segments) 
{ 
	glPushMatrix();
	float theta = 2 * 3.1415926 / float(num_segments); 
	float tangetial_factor = tanf(theta);//calculate the tangential factor 
 
	float radial_factor = cosf(theta);//calculate the radial factor 
 
	float x = r;//we start at angle = 0 
 
	float y = 0; 
 
	glBegin(GL_LINE_LOOP); 
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		glVertex2f(x + cx, y + cy);//output vertex 
 
		//calculate the tangential vector 
		//remember, the radial vector is (x, y) 
		//to get the tangential vector we flip those coordinates and negate one of them 
 
		float tx = -y; 
		float ty = x; 
 
		//add the tangential vector 
 
		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 
 
		//correct using the radial factor 
 
		x *= radial_factor; 
		y *= radial_factor; 
	} 
	glEnd(); 
	glPopMatrix();
}

void humanBody(){
	
	glPushMatrix();
	glRotatef(Kep_ang,Kep_rx,Kep_ry,Kep_rz);
	glTranslatef(Kep_x,Kep_y,Kep_z);
	glScalef(-.5,1,1);

	//head
	glPushMatrix();
	
	glTranslatef(0,0,13);
	glutSolidSphere(3,20,20);
	glPopMatrix();

	//body
	GLUquadric *obj = gluNewQuadric();
	gluCylinder(obj,4,4,10,20,20);

	//right hand
	glPushMatrix();
	glTranslated(7,0,4);
	glRotatef(-45,0,1,0);
	GLUquadric *righthand = gluNewQuadric();
	gluCylinder(righthand,1,2,6,20,20);
	glPopMatrix();

	//lefthand
	glPushMatrix();
	glTranslated(-7,0,4);
	glRotatef(45,0,1,0);
	GLUquadric *lefthand = gluNewQuadric();
	gluCylinder(lefthand,1,2,6,20,20);
	glPopMatrix();

	//right thi
	glPushMatrix();
	glTranslated(2,0,-7);
	//glRotatef(-30,0,1,0);
	GLUquadric *rightthi = gluNewQuadric();
	gluCylinder(rightthi,1,1.5,7,20,20);
	glPopMatrix();

	//left thi
	glPushMatrix();
	glTranslated(-2,0,-7);
	//glRotatef(-30,0,1,0);
	GLUquadric *leftthi = gluNewQuadric();
	gluCylinder(leftthi,1,1.5,7,20,20);
	glPopMatrix();


	glPopMatrix();
	

}


void gridAndAxis(){
	// draw the three major AXES
	if(canDrawAxis==1){
		glBegin(GL_LINES);
			//X axis
			glColor3f(0, 1, 0);	//100% Green
			glVertex3f(-150, 0, 0);
			glVertex3f( 150, 0, 0);
		
			//Y axis
			glColor3f(0, 0, 1);	//100% Blue
			glVertex3f(0, -150, 0);	// intentionally extended to -150 to 150, no big deal
			glVertex3f(0,  150, 0);
		
			//Z axis
			glColor3f(1, 1, 1);	//100% White
			glVertex3f( 0, 0, -150);
			glVertex3f(0, 0, 150);
		glEnd();
	}

	if(canDrawGrid == 1){
	//some gridlines along the field
		int i;

		glColor3f(0.5, 0.5, 0.5);	//grey
		glBegin(GL_LINES);
			for(i=-10;i<=10;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -100, 0);
				glVertex3f(i*10,  100, 0);

				//lines parallel to X-axis
				glVertex3f(-100, i*10, 0);
				glVertex3f( 100, i*10, 0);
			}
		glEnd();

	}
}

/********************************************************************************/

void display_ball(){
	glPushMatrix();
	glTranslatef(ball_x,ball_y,ball_z);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,football);
	glColor3f(1,1,1);
	GLUquadricObj *qobj = gluNewQuadric();
	gluQuadricTexture(qobj,GL_TRUE);
	gluSphere(qobj,1.5,20,20);
	
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}


void goalBar(double x, double y, double z){
	
	glPushMatrix();
	glColor3f(1,1,1);
	glTranslatef(x,y,z);

	glScalef(3,3,3);
	
	//left thum
	glPushMatrix();
	glTranslatef(-14.01,0,0);
	GLUquadric *left = gluNewQuadric();
	gluCylinder(left,.3,.3,15,20,20);
	glPopMatrix();

	//right thum
	glPushMatrix();
	glTranslated(14,0,0);
	GLUquadric *right = gluNewQuadric();
	gluCylinder(right,.3,.3,15,20,20);
	glPopMatrix();


	//top thum
	glPushMatrix();
	glTranslated(-14.3,0,15);
	glRotatef(180,1,0,1);
	GLUquadric *top = gluNewQuadric();
	gluCylinder(top,.3,.3,28.5,20,20);
	glPopMatrix();
	
	glPopMatrix();
}

void goalPastNet(double dx, double dy, double dz){

	glPushMatrix();
	glScalef(3,3,3);
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslatef(0,5,0);
	for(int i = 0,q = 1 ; i < 15 ; i++,q++){
		glBegin(GL_LINES); glVertex3f(-13,0,q); glVertex3f(13,0,q); glEnd();
	}

	for(int i = 0,q = -13 ; i < 27 ; i++,q++){
		glBegin(GL_LINES); glVertex3f(q,0,0); glVertex3f(q,0,15); glEnd();
	}
	glPopMatrix();



	glPushMatrix();
	glTranslatef(14,0,0);
	glRotatef(100,0,0,1);
	for(int i = 0,q = 1 ; i < 15 ; i++,q++){
		glBegin(GL_LINES); glVertex3f(0,0,q); glVertex3f(5,0,q); glEnd();
	}

	for(int i = 0,q = 0 ; i < 6 ; i++,q++){
		glBegin(GL_LINES); glVertex3f(q,0,0); glVertex3f(q,0,15); glEnd();
	}
	glPopMatrix();



	glPushMatrix();
	glTranslatef(-14,0,0);
	glRotatef(80,0,0,1);
	for(int i = 0,q = 1 ; i < 15 ; i++,q++){
		glBegin(GL_LINES); glVertex3f(0,0,q); glVertex3f(5,0,q); glEnd();
	}

	for(int i = 0,q = 0 ; i < 6 ; i++,q++){
		glBegin(GL_LINES); glVertex3f(q,0,0); glVertex3f(q,0,15); glEnd();
	}
	glPopMatrix();

	glPopMatrix();

	
}


void Ground(){

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,field);
	glBegin(GL_QUADS);

	glTexCoord2f(0,1);
	glVertex3f(120,80,2);
	glTexCoord2f(0,0);
	glVertex3f(-120,80,2);
	glTexCoord2f(1,0);
	glVertex3f(-120,-150,2);
	glTexCoord2f(1,1);
	glVertex3f(120,-150,2);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glRotatef(180,0,1,0);
	glTranslatef(0,0,-130);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,stadium);
	glBegin(GL_QUADS);

	glTexCoord2f(1,0);
	glVertex3f(120,110,150);
	glTexCoord2f(1,1);
	glVertex3f(120,110,2);
	glTexCoord2f(0,1);
	glVertex3f(-120,110,2);
	glTexCoord2f(0,0);
	glVertex3f(-120,110,150);
	
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();



	glPushMatrix();
	glRotatef(90,0,0,1);
	glPushMatrix();
	glRotatef(180,0,1,0);
	glTranslatef(0,0,-130);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,stadium);
	glBegin(GL_QUADS);

	glTexCoord2f(1,0);
	glVertex3f(120,110,150);
	glTexCoord2f(1,1);
	glVertex3f(120,110,2);
	glTexCoord2f(0,1);
	glVertex3f(-120,110,2);
	glTexCoord2f(0,0);
	glVertex3f(-120,110,150);
	
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	glPopMatrix();



	glPushMatrix();
	glRotatef(-90,0,0,1);
	glPushMatrix();
	glRotatef(180,0,1,0);
	glTranslatef(0,0,-130);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,stadium);
	glBegin(GL_QUADS);

	glTexCoord2f(1,0);
	glVertex3f(120,110,150);
	glTexCoord2f(1,1);
	glVertex3f(120,110,2);
	glTexCoord2f(0,1);
	glVertex3f(-120,110,2);
	glTexCoord2f(0,0);
	glVertex3f(-120,110,150);
	
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	glPopMatrix();

}


void point(){
	glPushMatrix();
	glColor3f(.5,0,0);
	glTranslatef(point_x,-1,point_z);
	glutSolidCube(3);
	glPopMatrix();
}

void backBorder(double dx, double dy, double dz,double sx, double sy, double sz,double cx, double cy, double cz){
	glPushMatrix();
	glColor3f(cx,cy,cz);
	glTranslated(dx,dy,dz);
	glScaled(sx,sy,sz);
	glutSolidCube(1);
	glPopMatrix();
	
}

void backBorder2(double dx, double dy, double dz,double sx, double sy, double sz){
	glPushMatrix();
	glColor3f(cc_xx,cc_yy,cc_zz);
	glTranslated(dx,dy,dz);
	glScaled(sx,sy,sz);
	glutSolidCube(1);
	glPopMatrix();
	
}

void goalpoint(double dx, double dy, double dz){
	glPushMatrix();
	glColor3f(.9,.9,.9);
	glTranslated(dx,dy,dz);
	//glRotatef(25,1,0,0);
	glutSolidTorus(.4,1,20,20);
	glPopMatrix();
}

void drawText(const char *text, int length, int x, int y){
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX,matrix);
	glLoadIdentity();
	//glOrtho(0,800,0,600,-5,5);
	glOrtho(500,500,500,500,500,500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x,y);
	glColor3f(1,0,0);
	for(int i = 0; i < length; i++){
		//glutBitmapCharacter(GLUT_BITMAP_9_BY_15,(int)text[i]);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text[i]);
	}

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);

	glMatrixMode(GL_MODELVIEW);
}



void display(){
	//codes for Models, Camera
	
	//clear the display
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//clear buffers to preset values

	/***************************
	/ set-up camera (view) here
	****************************/ 
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);		//specify which matrix is the current matrix

	//initialize the matrix
	glLoadIdentity();				//replace the current matrix with the identity matrix [Diagonals have 1, others have 0]

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(0,-150,20,	0,0,0,	0,0,1);
	//gluLookAt(cameraRadius*sin(cameraAngle), -cameraRadius*cos(cameraAngle), cameraHeight,		0,0,0,		0,0,1);

	V to=loc+dir;
	gluLookAt(loc.x, loc.y, loc.z,		to.x,to.y,to.z,		perp.x,perp.y,perp.z);
	
	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/**************************************************
	/ Grid and axes Lines(You can remove them if u want)
	***************************************************/
	
	gridAndAxis();
	
	
	/****************************
	/ Add your objects from here
	****************************/
	
	
	Ground();
	goalBar(0,0,0);
	display_ball();

	point();

	goalPastNet(0,0,0);
	humanBody();

	goalpoint(0,-110,3);
	
	//Ground design
	backBorder(0,1,0,180,1,5,1,1,1);
	backBorder(0,-60,0,110,1,5,1,1,1);
	backBorder(55,-30,0,.3,60,5,1,1,1);
	backBorder(-55,-30,0,.3,60,5,1,1,1);

	//3 pilars
	backBorder2(0,65,0,200,2,15);
	backBorder2(100,10,0,2,100,15);
	backBorder2(-100,10,0,2,100,15);
	glPushMatrix();
	glTranslatef(0,-50,50);
	glColor3f(1,0,0);
	std:: string str = "7488489";
	drawText(str.data(),str.size(),0,0);
	//glPopMatrix();

	
	
	
	

	
	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glFlush();
	glutSwapBuffers();
}
		

void animate(){

	if(neg && shotSpeed > 20) shotSpeed -= 10;
	if(pos && shotSpeed < 140) shotSpeed += 10;
	if(p){
		point_x += .1;
		if(point_x > 60) p = 0;
	}
	else{
		point_x -= .1;
		if(point_x < -60) p = 1;
	}
	if(correctSot == 5 ){
		if(random_num == 0 || random_num == 3){
			if(go){
				if(direction == 1 ){
					ball_x += abs(initBall_xx - secBall_xx)/shotSpeed;
					ball_y += abs(initBall_yy - secBall_yy)/shotSpeed;
					Kep_x += abs(initKep_xx -secKep_xx)/shotSpeed;
					if(ball_x >= secBall_xx) go = 0, goo = 1;
				}
				else{
					ball_x -= abs(initBall_xx - secBall_xx)/shotSpeed;
					ball_y += abs(initBall_yy - secBall_yy)/shotSpeed;
					Kep_x -= abs(initKep_xx -secKep_xx)/shotSpeed;
					if(ball_x <= secBall_xx) go = 0, goo = 1;
				}
			}
			if(goo){
				ball_y -= abs(thiBall_yy - secBall_yy)/shotSpeed;
				ball_z -= abs(thiBall_zz - secBall_zz)/shotSpeed;
				if(ball_y <= thiBall_yy) goo = 0;
			}
		}
		if(random_num == 1){
 			if(go){
				if(direction == 1 ){
					ball_x += abs(initBall_xx - secBall_xx)/shotSpeed;
					ball_y += abs(initBall_yy - secBall_yy)/shotSpeed;
					Kep_x -= abs(initKep_xx -secKep_xx)/shotSpeed;
					if(ball_x >= secBall_xx) go = 0, goo = 1;
				}
				else{
					
					ball_x -= abs(initBall_xx - secBall_xx)/shotSpeed;
					ball_y += abs(initBall_yy - secBall_yy)/shotSpeed;
					Kep_x += abs(initKep_xx -secKep_xx)/shotSpeed;
					if(ball_x <= secBall_xx) go = 0, goo = 1;
				}
			}
			if(goo){
				ball_y -= abs(thiBall_yy - secBall_yy)/shotSpeed;
				ball_z -= abs(thiBall_zz - secBall_zz)/shotSpeed;
				if(ball_y <= thiBall_yy) goo = 0;
			}
		}
		if(random_num == 2){
			if(go){
				if(direction == 1 ){
					ball_x += abs(initBall_xx - secBall_xx)/shotSpeed;
					ball_y += abs(initBall_yy - secBall_yy)/shotSpeed;
					Kep_x += abs(initKep_xx -secKep_xx)/shotSpeed;
					if(ball_x >= secBall_xx) go = 0, goo = 1;
				}
				else{
					ball_x -= abs(initBall_xx - secBall_xx)/shotSpeed;
					ball_y += abs(initBall_yy - secBall_yy)/shotSpeed;
					Kep_x -= abs(initKep_xx -secKep_xx)/shotSpeed;
					if(ball_x <= secBall_xx) go = 0, goo = 1;
				}
			}
			if(goo){
				ball_y -= abs(thiBall_yy - secBall_yy)/shotSpeed;
				ball_z -= abs(thiBall_zz - secBall_zz)/shotSpeed;
				if(ball_y <= thiBall_yy) goo = 0;
			}
		}

	}


	if(correctSot == 1 || correctSot == -1 ){
		if(go){
			if(direction == 1 ){
				ball_x += abs(initBall_xx - secBall_xx)/shotSpeed;
				ball_y += abs(initBall_yy - secBall_yy)/shotSpeed;
				Kep_x += abs(initKep_xx -secKep_xx)/shotSpeed;
				if(ball_x >= secBall_xx) go = 0, goo = 1;
			}
			else{
				ball_x -= abs(initBall_xx - secBall_xx)/shotSpeed;
				ball_y += abs(initBall_yy - secBall_yy)/shotSpeed;
				Kep_x -= abs(initKep_xx -secKep_xx)/shotSpeed;
				if(ball_x <= secBall_xx) go = 0, goo = 1;
			}
		}
		if(goo){
			cc_xx = 1;
			cc_yy = 0;
			ball_y -= abs(thiBall_yy - secBall_yy)/shotSpeed;
			ball_z -= abs(thiBall_zz - secBall_zz)/shotSpeed;
			if(ball_y <= thiBall_yy) goo = 0;
		}
	}
	
	//codes for any changes in Models, Camera
	
	//cameraAngle += cameraAngleDelta;	// camera will rotate at 0.002 radians per frame.
	
	//codes for any changes in Models

	//MISSING SOMETHING? -- YES: add the following
	glutPostRedisplay();	//this will call the display AGAIN

}

void init(){
	//codes for initialization
	loadImage();
	
	cameraAngle = 0;	//angle in radian
	move_X = 0;
	move_Y = 0;
	move_Z = 0;
	canDrawGrid = 1;
	canDrawAxis = 1;

	//MY INITIALIZATION
	point_x = 0;
	point_z = 8;
	Kep_x = initKep_xx = 0;
	Kep_y = initKep_yy = -3;
	Kep_z = 10;

	shotSpeed = 80;
	ball_x = initBall_xx = 0;
	ball_y = initBall_yy = -110;
	ball_z = initBall_zz = 5;

	Kep_rx = 0,Kep_ry=0 ,Kep_rz = 0;
	Kep_ang = 0;

	cc_yy = 1;

	//cameraAngleDelta = .001;
	cameraAngleDelta = 0;

	cameraRadius = 150;
	cameraHeight = 50;


	loc=_L,dir=_D,perp=_P;
	//clear the screen
	glClearColor(0,0,0, 0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);
	
	//initialize the matrix
	glLoadIdentity();

	/*
		gluPerspective() — set up a perspective projection matrix

		fovy -         Specifies the field of view angle, in degrees, in the y direction.
        aspect ratio - Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
        zNear -        Specifies the distance from the viewer to the near clipping plane (always positive).
        zFar  -        Specifies the distance from the viewer to the far clipping plane (always positive).
    */
	
	gluPerspective(70,	1,	0.1,	10000.0);
	
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

	case '5':
		//if(-3 > -2) printf("yes");
		go = 1;
		point_z = -10;
		point_X = point_x;
		if(point_X >= 0) direction = 1;
		else direction = -1;

		if(point_X > -42.5 && point_X < 42.5) correctSot = 5;
		else if(point_X < -42.5) correctSot = -1;
		else if(point_X > 42.5 ) correctSot = 1;
		//else if (point_X <= -43.5 && point_X >= -43.5) correctSot = -2;
		//else if (point_X >= 43.5 && point_X <= 43.5) correctSot = 2;
		//correctSot = -1;
		//printf("%lf",point_X);
		if(correctSot == 5){ 
				random_num = rand()%4;
				
				secBall_xx = point_X;
				if(random_num == 0 || random_num == 3 ) secBall_yy = 0;
				else secBall_yy = 8;
				secBall_zz	= 5;

				thiBall_xx = secBall_xx;
				if(random_num == 0 || random_num == 3) thiBall_yy = secBall_yy - 50;
				else thiBall_yy = secBall_yy - 10;
				thiBall_zz = 5;

				if(random_num == 0 || random_num == 3 ) secKep_xx = point_X;
				else if(random_num == 1 )	secKep_xx = (point_X / 3)*2;
				else{
					if(direction == 1)
						secKep_xx = abs(point_x - initKep_xx)/3 * 2;
					else 
						secKep_xx = abs(point_x - initKep_xx)/3 * 2 * -1;
				}


				secKep_yy = 0;
				secKep_zz = 12;

		}

		if(correctSot == 1){
			//printf("right");
				//cc_xx = 1;
				//cc_yy = 0;
			 
				secBall_xx = point_X + 20;
				secBall_yy = 64;
				secBall_zz	= 5;

				thiBall_xx = secBall_xx;
				thiBall_yy = secBall_yy - 70;
				thiBall_zz = 5;

				secKep_xx = 30;
				secKep_yy = 0;
				secKep_zz = 12;
		}

		if(correctSot == -1){
			cc_xx = 1;
			cc_yy = 0;
			 
				secBall_xx = point_X - 20;
				secBall_yy = 64;
				secBall_zz	= 5;

				thiBall_xx = secBall_xx;
				thiBall_yy = secBall_yy - 70;
				thiBall_zz = 5;

				secKep_xx = -30;
				secKep_yy = 0;
				secKep_zz = 12;
		}
		
		
		break;

	case '0':
		cc_xx = 0;
		cc_yy = 1;
		point_z = 8;
		Kep_x = initKep_xx = 0;
		Kep_y = initKep_yy = -3;
		Kep_z = 10;

		ball_x = initBall_xx = 0;
		ball_y = initBall_yy = -110;
		ball_z = initBall_zz = 5;
		
		break;

	case 'w':case 'W':
		loc=loc+speed*dir;
		break;

	case 'a':case 'A':
		loc=loc+speed*((perp*dir).unit());
		break;

	case 's':case 'S':
		loc=loc-speed*dir;
		break;

	case 'd':case 'D':
		loc=loc+speed*((dir*perp).unit());
		break;

	case 'q':case 'Q':
		perp=perp.rot(dir,ang_speed);
		break;

	case 'e':case 'E':
		perp=perp.rot(-dir,ang_speed);
		break;
	case '-':
		pos = 1;
		neg = 0;
		break;
	case '+':
		pos = 0;
		neg = 1;
		break;
	
	default:
		break;
	}
}

void specialKeyListener(int key, int x,int y){
	V p;

	switch(key){
	case GLUT_KEY_UP:
		p=(dir*perp).unit();
		dir=dir.rot(p,ang_speed);
		perp=perp.rot(p,ang_speed);
		break;
	case GLUT_KEY_DOWN:
		p=(perp*dir).unit();
		dir=dir.rot(p,ang_speed);
		perp=perp.rot(p,ang_speed);
		break;

	case GLUT_KEY_LEFT:
		dir=dir.rot(-perp,ang_speed);
		break;
	case GLUT_KEY_RIGHT:
		dir=dir.rot(perp,ang_speed);
		break;

	case GLUT_KEY_PAGE_UP:
		loc=loc+speed*perp;
		break;
	case GLUT_KEY_PAGE_DOWN:
		loc=loc-speed*perp;
		break;

	case GLUT_KEY_INSERT:
		break;

	case GLUT_KEY_HOME:
		loc=_L,dir=_D,perp=_P;
		break;
	case GLUT_KEY_END:
		break;

	default:
		break;
	}
}

void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}

int main(int argc, char **argv){
	
	
	glutInit(&argc,argv);							//initialize the GLUT library
	
	glutInitWindowSize(1000, 800);
	glutInitWindowPosition(100, 100);
	
	/*
		glutInitDisplayMode - inits display mode
		GLUT_DOUBLE - allows for display on the double buffer window
		GLUT_RGBA - shows color (Red, green, blue) and an alpha
		GLUT_DEPTH - allows for depth buffer
	*/
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	

	glutCreateWindow("Some Title");

	printf("Camera Control\n");
	printf("_____________\n");
	printf("Roll: UP and DOWN arrow\n");
	printf("Pitch: Q and E\n");
	printf("Yaw: LEFT and RIGHT arrow\n");
	printf("Up-Down: PAGEUP and PAGEDOWN\n");
	printf("Left-Right: A and D\n");
	printf("Zoom in-out: W and S\n");
	printf("Reset Camera: HOME\n");
	

	init();						//codes for initialization

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	

	return 0;
}
