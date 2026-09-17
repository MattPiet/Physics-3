#ifndef TRACKBALL_H
#define	TRACKBALL_H
#include <Vector.h>
#include <Quaternion.h>
union SDL_Event;



	class Trackball {
	private:
		bool mouseDown;
		MATH::Quaternion mouseRotationQuat;
		// UN - keep track of the orientation before we start rotating things
		MATH::Quaternion prevQuat;
		MATH::Matrix4 invNDC;			/// the inverse of the viewportNDC matrix
		MATH::Vec3 beginV, endV;		/// Begin and end points after being transformed by invNDC
		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
		int lastMouseX, lastMouseY;
		float m_Sensitivity = 0.1f;
	public:
		bool Trackingx = true;
		bool Trackingz = true;
		Trackball(); 
		~Trackball();
		Trackball(const Trackball&) = delete;
		Trackball(Trackball&&) = delete;
		Trackball& operator=(const Trackball&) = delete;
		Trackball& operator=(Trackball&&) = delete;

		const MATH::Quaternion getQuat() const { return mouseRotationQuat; }
		void HandleEvents(const SDL_Event &sdlEvent);
		void SetSensitivity(float s) { m_Sensitivity = s;}
	private:
		/// Just some functions for internal use only thus private
		const MATH::Vec3 getMouseVector(int x, int y);
		///Any time the window dimensions change like stretching it we will
		/// need to rebuild the invNDC matrix
		void setWindowDimensions();
		void onRightMouseDown(int x, int y);
		void onRightMouseUp(int x, int y);
		void onMouseMove(int x, int y);
	};

#endif

