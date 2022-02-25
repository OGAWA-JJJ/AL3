#pragma once
#include <cmath>

namespace OgaJEase {

float PI = 3.14159265359;


/*-----Sine-----*/

float easeInSine(float t) {
	return 1 - cosf((t * PI) / 2);
}

float easeOutSine(float t) {
	return sinf((t * PI) / 2);
}

float easeInOutSine(float t) {
	return -(cosf(PI * t) - 1) / 2;
}


/*-----Quad-----*/

float easeInQuad(float t) {
	return t * t;
}

float easeOutQuad(float t) {
	return 1 - (1 - t) * (1 - t);
}

float easeInOutQuad(float t) {
	return t < 0.5 ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2;
}


/*-----Cubic-----*/

float easeInCubic(float t) {
	return t * t * t;
}

float easeOutCubic(float t) {
	return 1 - powf(1 - t, 3);
}

float easeInOutCubic(float t) {
	return t < 0.5 ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2;
}


/*-----Quart-----*/

float easeInQuart(float t) {
	return t * t * t * t;
}

float easeOutQuart(float t) {
	return 1 - powf(1 - t, 4);
}

float easeInOutQuart(float t) {
	return t < 0.5 ? 8 * t * t * t * t : 1 - powf(-2 * t + 2, 4) / 2;
}

}