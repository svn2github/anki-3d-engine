#ifndef ANKI_EVENT_SCENE_COLOR_EVENT_H
#define ANKI_EVENT_SCENE_COLOR_EVENT_H

#include "anki/event/Event.h"
#include "anki/math/Math.h"


namespace anki {


/// Change the scene color
class SceneColorEvent: public Event
{
	public:
		/// Constructor
		SceneColorEvent(float startTime, float duration,
			const Vec3& finalColor);

		/// Copy constructor
		SceneColorEvent(const SceneColorEvent& b);

		/// Copy
		SceneColorEvent& operator=(const SceneColorEvent& b);

	private:
		Vec3 originalColor; ///< Original scene color. The constructor sets it
		Vec3 finalColor;

		/// Implements Event::updateSp
		void updateSp(float prevUpdateTime, float crntTime);
};


} // end namespace


#endif
