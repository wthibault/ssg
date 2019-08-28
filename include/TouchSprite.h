//
// TouchSprite.h
//  -- an ssg Instance that poll()s a Touchpad and controls some geometry,etc.
//
#include "Touchpad.h"
#include "ssg.h"

//
// TouchSpriteController 
// and manages a set of TouchSprites, and polls the touchpad
//
class TouchSpriteController : public ssg::Instance
{
 public:
  TouchSpriteController ( int _numSprites ); // create the TouchSprites and add a children of this.
  ~TouchSpriteController();

  virtual void update ( float dt );

  ssg::Ptr<Touchpad> pad; // poll() this to get touch data
  std::vector<Ptr<TouchSprite> > sprites; // these are all children of this ssg::Instance
  std::vector<TouchpadContact> allContacts;  // an array of data per sprite (not same as raw Touchpad report)



};


class TouchSprite : public ssg::Instance
{
 public:

  TouchSprite ( int _id );
  ~TouchSprite();

  virtual void update ( float dt );

  int id;
  
};
