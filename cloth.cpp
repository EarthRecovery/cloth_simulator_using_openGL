#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

#define SPRING_NUMBER 
#define SPRING_CONSTANT 35.0f
#define LENGTH 20
#define HEIGHT 20
#define PASSED_TIME 20
#define DRAG_COEFFICIENT 1.8f
#define DAMP_FACTOR 0.15f

struct Point {
	glm::vec3 position;
	float mass;
	bool fixed;
	glm::vec3 force;
	glm::vec3 velocity;

	Point() {
		position = glm::vec3{ 0.0f,0.0f,0.0f };
		mass = 0.1f;
		fixed = false;
		force = glm::vec3{ 0.0f,0.0f,0.0f };
		velocity = glm::vec3{ 0.0f,0.0f,0.0f };
	}
};

struct Spring {
	int ax;
	int ay;
	int bx;
	int by;
	float constant;
	float length;

	Spring() {
		constant = SPRING_CONSTANT;
		length = 0.0f;
	}
};

class Cloth {
private:
	std::vector<Spring> SpringArray;

	int springNumber(int height, int width) {
		int res = 0;
		res += (height) * (width - 1) + (height - 1) * (width); //struct
		res += 2 * (height - 1) * (width - 1);  //cut
		res += (height) * (width - 2) + (height - 2) * (width); //band

		std::cout << "spawn " << res << "structures" << std::endl;
		return res;
	}

	void initialize() {
		// initialize point
		for (int i = 0;i < HEIGHT;i++) {
			for (int j = 0;j < LENGTH;j++) {
				CurrentPointArray[i][j].position = glm::vec3(j * 0.1f, i * 0.1f, 0.0f);
				NextPointArray[i][j].position = glm::vec3(j * 0.1f, i * 0.1f, 0.0f);
			}
		}
		CurrentPointArray[0][0].fixed = true;
		CurrentPointArray[0][LENGTH-1].fixed = true;
		CurrentPointArray[HEIGHT-1][0].fixed = true;
		CurrentPointArray[HEIGHT-1][LENGTH-1].fixed = true;

		//initialize spring - struct width
		int springTop = 0;
		for (int i = 0;i < HEIGHT;i++) {
			for (int j = 0;j < LENGTH-1;j++) {
				SpringArray[springTop].ax = i;
				SpringArray[springTop].ay = j;
				SpringArray[springTop].bx = i;
				SpringArray[springTop].by = j + 1;
				SpringArray[springTop].length = 0.1f;
				springTop++;
			}
		}

		//initialize spring - struct height
		for (int i = 0;i < HEIGHT-1;i++) {
			for (int j = 0;j < LENGTH;j++) {
				SpringArray[springTop].ax = i;
				SpringArray[springTop].ay = j;
				SpringArray[springTop].bx = i + 1;
				SpringArray[springTop].by = j;
				SpringArray[springTop].length = 0.1f;
				springTop++;
			}
		}


		//initialize spring - cut down
		for (int i = 0;i < HEIGHT - 1;i++) {
			for (int j = 0;j < LENGTH - 1;j++) {
				SpringArray[springTop].ax = i;
				SpringArray[springTop].ay = j;
				SpringArray[springTop].bx = i + 1;
				SpringArray[springTop].by = j + 1;
				SpringArray[springTop].length = std::sqrt(0.02f);
				springTop++;
			}
		}


		//initialize spring - cut up
		for (int i = HEIGHT - 1;i >= 1;i--) {
			for (int j = 0;j < LENGTH - 1;j++) {
				SpringArray[springTop].ax = i;
				SpringArray[springTop].ay = j;
				SpringArray[springTop].bx = i - 1;
				SpringArray[springTop].by = j + 1;
				SpringArray[springTop].length = std::sqrt(0.02f);
				springTop++;
			}
		}


		//initialize spring - band width
		for (int i = 0;i < HEIGHT;i++) {
			for (int j = 0;j < LENGTH - 2;j++) {
				SpringArray[springTop].ax = i;
				SpringArray[springTop].ay = j;
				SpringArray[springTop].bx = i;
				SpringArray[springTop].by = j + 2;
				SpringArray[springTop].length = 0.2f;
				springTop++;
			}
		}


		//initialize spring - cut height
		for (int i = 0;i < HEIGHT - 2;i++) {
			for (int j = 0;j < LENGTH;j++) {
				SpringArray[springTop].ax = i;
				SpringArray[springTop].ay = j;
				SpringArray[springTop].bx = i + 2;
				SpringArray[springTop].by = j;
				SpringArray[springTop].length = 0.2f;
				springTop++;
			}
		}

	}

	float getLength(Point a, Point b) {
		return std::sqrt(std::pow(b.position.x - a.position.x, 2) + std::pow(b.position.y - a.position.y, 2)
			+ std::pow(b.position.z - a.position.z, 2));
	}

	glm::vec3 getDirection(Point a, Point b) {
		return glm::normalize(a.position - b.position);
	}
public:
	Point CurrentPointArray[HEIGHT][LENGTH];
	Point NextPointArray[HEIGHT][LENGTH];
	Cloth() {
		SpringArray = std::vector<Spring>(springNumber(HEIGHT, LENGTH));
		initialize();
	}

	void calculateForce() {
		for (auto& spring : SpringArray) {
			float tension = spring.constant * (getLength(CurrentPointArray[spring.ax][spring.ay], CurrentPointArray[spring.bx][spring.by]) - spring.length)
				/ spring.length;
			glm::vec3 FVD = (CurrentPointArray[spring.bx][spring.by].velocity - CurrentPointArray[spring.ax][spring.ay].velocity) * DRAG_COEFFICIENT;
			glm::vec3 force = getDirection(CurrentPointArray[spring.bx][spring.by], CurrentPointArray[spring.ax][spring.ay]) * tension;
			CurrentPointArray[spring.ax][spring.ay].force += force;
			CurrentPointArray[spring.ax][spring.ay].force += FVD;

			FVD = (CurrentPointArray[spring.ax][spring.ay].velocity - CurrentPointArray[spring.bx][spring.by].velocity) * DRAG_COEFFICIENT;
			force = getDirection(CurrentPointArray[spring.ax][spring.ay], CurrentPointArray[spring.bx][spring.by]) * tension;
			CurrentPointArray[spring.bx][spring.by].force += force;
			CurrentPointArray[spring.bx][spring.by].force += FVD;
		}
		
		for (int i = 0;i < HEIGHT;i++) {
			for (int j = 0;j < LENGTH;j++) {
				CurrentPointArray[i][j].force += glm::vec3(0.0f, 0.0f, -1.0f);
			}
		}
	}

	void calculateVelocity() {
		for (int i = 0;i < HEIGHT;i++) {
			for (int j = 0;j < LENGTH;j++) {
				if (CurrentPointArray[i][j].fixed) {
					CurrentPointArray[i][j].velocity = glm::vec3{ 0.0f,0.0f,0.0f };
					NextPointArray[i][j].position = CurrentPointArray[i][j].position;
					NextPointArray[i][j].fixed = true;
					continue;
				}
				glm::vec3 acceleration = CurrentPointArray[i][j].force / CurrentPointArray[i][j].mass;
				NextPointArray[i][j].velocity = CurrentPointArray[i][j].velocity + acceleration * (float)PASSED_TIME / 1000.0f;
				NextPointArray[i][j].velocity *= DAMP_FACTOR;
				NextPointArray[i][j].position = CurrentPointArray[i][j].position +
					(NextPointArray[i][j].velocity + CurrentPointArray[i][j].velocity) / 2.0f * (float)PASSED_TIME / 1000.0f;
			}
		}
	}

	void updatePosition() {
		for (int i = 0;i < HEIGHT;i++) {
			for (int j = 0;j < LENGTH;j++) {
				CurrentPointArray[i][j] = NextPointArray[i][j];
				CurrentPointArray[i][j].force = glm::vec3{ 0.0f, 0.0f, 0.0f };
			}
		}
	}

	void changePointPosition(float x,float y, float z) {
		CurrentPointArray[0][0].position.x = x;
		CurrentPointArray[0][0].position.y = y;
		CurrentPointArray[0][0].position.z = z;
	}

	void simulate() {
		calculateForce();
		calculateVelocity();
		updatePosition();
	}

	void run() {
		simulate();
	}
};