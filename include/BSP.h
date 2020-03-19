//
// BSP.h
//
// started March 2020
// Bill Thibault
//

#pragma once
#include "ssg.h"

namespace ssg {

  class BSPPrimitive : public ssg::Primitive {

  public:

    // Each vertex in a Tri is an index into Primitive::points_
    typedef glm::ivec3       Tri;

    // Tris for lists of tris.  int index refers to points_
    typedef std::vector<Tri> Tris;

    // for the BSP tree proper
    // try to be lightweight
    struct BSP {
      glm::vec4               Hp;
      BSP                     *front, *back;
      Tris                    inTris;
    };
    
    BSPPrimitive();

    // Build a BSP tree from a list of triangles.
    // Uses args to init points_ et alia.
    // Triangles split during process will generate add'l vertices.
    void buildBSP ( const std::vector<vec3> &positions,
		    const std::vector<vec3> &normals,
		    const std::vector<vec2> &texCoords,
		    const Tris &tris );

    // Build a BSP tree by traversing a scene graph,
    // creating copies of all (possibly transformed)
    // triangles, 
    // and set these as starting points_, et al. for the BSP tree.
    // More points are added if any splitting occurs.
    // XXX monitor the graph for changes and rebuild tree in update()? 
    void buildBSP ( Ptr<ModelNode> );   

    virtual ~BSPPrimitive();

    virtual void init();
    virtual void update (float dt);
    virtual void draw ( glm::mat4 mv,
			glm::mat4 proj,
			Ptr<ssg::Material> mat);

    BSP *getBSPTree ();
    
  protected:

    BSP *bspTree;

    glm::vec4 chooseHp ( const Tris &tris );

    void      splitTris ( const Tris      &tris,
			  const glm::vec4 Hp,
			  Tris      &frontTris,
			  Tris      &backTris );

    void      splitTri ( const Tri tri,
			 const glm::vec4 Hp,
			 const glm::vec3 d,
			 std::pair<Tri,Tri> front,
			 std::pair<Tri,Tri> back );
    

    glm::vec3 findIntersection ( const glm::vec3 p1,
				 const glm::vec3 p2,
				 const glm::vec4 Hp );

    // XXX can we move these funcs somewhere like BSP, to avoid vtable lookup?

    glm::vec3      evalHp ( const glm::vec4 Hp, const Tri tri );
    bool           allZero ( const glm::vec3 d );
    bool           allNonZero ( const glm::vec3 d );
    bool           allToOneSide ( const glm::vec3 d, const int side );
    void           appendIfExists ( const Tri tri, Tris &tris );
    int            addNewPoint ( const glm::vec3 p );
    
    
  };

}
