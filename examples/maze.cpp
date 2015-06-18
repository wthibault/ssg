//
// 
// shows how to subclass ssg::Primitive to create a static random maze
//

#include "ssg.h"
#include <cmath>
#include <vector>
using namespace glm;
using namespace ssg;

Ptr<Instance> root;
Ptr<Primitive> prim;
int width, height;

enum Mode { GOD_MODE, MAZE_MODE };
Mode  mode;

Camera     godCamera;
class MazeCamera;
MazeCamera *mazeCamera;

///////////////////////////////////////////////////////
// Maze is a subclass of Primitive.
// (see also the code for the Triangle and Marker in <SSG-installation>/src )
//
// The constructor should push , for each vertex of the primitive:
//  vec3 vertex coordinates onto the Primitive::points_ vector,
//  vec3 normals onto    normals_;
//  vec2 texture coordinates    texCoords_;
// The indices_ vector holds a list of indices of vertices, 
//     in the order required by the primitive type
// The primitive type is the GL_ constant for the drawing primitive to use.
//
// The constructor, after loading the above vectors,
// then calls Primitive::init(), which allocates and loads the 
// vertex array and index arrays.
//
// Once init() returns,  the application can call Primitive::draw()
// to draw the object, once a Material and its shaders have been
// created.
//
// For a dynamic object, you need to get down and dirty and
// use glBufferSubData to upload the new vertex attributes or element array.
// See the code for ParticleSystem.  You can subclass Particle system,
// and use its init() and even its draw() but you have to arrange to change 
//  the points_, normals_ , etc. vectors in your own code. see ParticleSystem::update()
// as an example.
//
// This example builds a graph of nodes that represent valid character 
// positions.  The nodes are placed in a regular grid of rows and columns,
// each node has 4 neighbors, in the N S E W directions. 
// Where there is no edge connecting the node to a neighbors,
// a wall is placed perpendicular to the missing edge.  The wall
// consists of two triangles forming a rectangle.
// Building the walls is done by traversing the nodes in raster order,
// and creating a wall if the E edge is missing, or the S edge is missing.
// The graph of nodes can be used to plan NPC placement and motion,
// although the stochastic choice of missing edges may make the maze
// ill-formed ie >1 connected component, etc.
// 
///////////////////////////////////////////////////////

double getNow() {
	return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0;
}

struct Node {
	int  id;
	vec3 position;
	int  neighbors[4]; // ordered ENWS
	Node(int id_) : id(id_), position(0, 0, 0) {
		for (int i = 0; i < 4; i++) neighbors[i] = -1;
	}
};

class Maze : public Primitive
{
private:
	int          _rows;
	int          _cols;
	float        _height;
	float        _spacing;
	float        _density;
	std::vector<Node> _nodes;

	int _indexOf(int row, int col) {
		return row * _cols + col;
	}

public:

	virtual void update(float dt) {}

	std::vector<Node> &getNodes() { return _nodes; }
	float              getHeight() { return _height; }

	Maze(int rows, int cols, float spacing, float wallHeight, float density)
		: _rows(rows), _cols(cols), _spacing(spacing), _height(wallHeight),
		_density(density)
	{

		// 
		// first create the graph of valid positions (maze interior)
		//

		// create the nodes

		vec3 corner(-_rows / 2 * _spacing, 0.0f, -_cols / 2 * _spacing);
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				Node n(_indexOf(row, col));
				n.position = corner + vec3(col*_spacing, 0.0f, row*_spacing);
				_nodes.push_back(n);
			}
		}

		// create the edges

		for (int row = 0; row < _rows; row++) {
			for (int col = 0; col < _cols; col++) {
				int id = _indexOf(row, col);
				if (urand() < _density && col < _cols - 1) {
					_nodes[id].neighbors[0] = _indexOf(row, col + 1);
					_nodes[_indexOf(row, col + 1)].neighbors[2] = id;
				}
				if (urand() < _density && row < _rows - 1) {
					_nodes[id].neighbors[3] = _indexOf(row + 1, col);
					_nodes[_indexOf(row + 1, col)].neighbors[1] = id;
				}
			}
		}

		//
		// create the maze walls between unconnected graph nodes
		//

		for (int row = 0; row < _rows; row++) {
			for (int col = 0; col < _cols; col++) {
				Node n = _nodes[_indexOf(row, col)];
				vec3 pos = n.position;
				vec3 midpoint, offset;
				if (n.neighbors[0] == -1) {
					midpoint = pos + vec3(_spacing / 2, 0, 0);
					offset = vec3(0, 0, _spacing / 2);
					addWall(midpoint, offset, _height);
				}
				if (n.neighbors[3] == -1) {
					midpoint = pos + vec3(0, 0, _spacing / 2);
					offset = vec3(_spacing / 2, 0, 0);
					addWall(midpoint, offset, _height);
				}
				// handle the top and left maze sides
				if (col == 0) {
					midpoint = pos - vec3(_spacing / 2, 0, 0);
					offset = vec3(0, 0, _spacing / 2);
					addWall(midpoint, offset, _height);
				}
				if (row == 0) {
					midpoint = pos - vec3(0, 0, _spacing / 2);
					offset = vec3(_spacing / 2, 0, 0);
					addWall(midpoint, offset, _height);
				}
			}
		}

		// add a base
		vec3 basecorner = corner - vec3(_spacing / 2, 0, _spacing / 2);
		vec3 EWside(_spacing * (_cols), 0, 0);
		vec3 NSside(0, 0, _spacing * (_rows));
		points_.push_back(basecorner);
		points_.push_back(basecorner + EWside);
		points_.push_back(basecorner + EWside + NSside);
		points_.push_back(basecorner + NSside);

		vec3 normal(0, 1, 0);
		for (int i = 0; i < 4; i++)
			normals_.push_back(normal);

		vec3 color(0, 1, 0);
		for (int i = 0; i < 4; i++)
			colors_.push_back(color);

		int i = points_.size() - 4;
		indices_.push_back(i);
		indices_.push_back(i + 1);
		indices_.push_back(i + 2);
		indices_.push_back(i);
		indices_.push_back(i + 2);
		indices_.push_back(i + 3);

		drawingPrimitive_ = GL_TRIANGLES;
		ssg::Primitive::init();
	}

	void addWall(vec3 midpoint, vec3 offset, float height) {
		vec3 v0 = midpoint + offset;
		vec3 v1 = midpoint - offset;
		vec3 v2 = v0 + vec3(0, height, 0);
		vec3 v3 = v1 + vec3(0, height, 0);

		points_.push_back(v0);
		points_.push_back(v1);
		points_.push_back(v2);
		points_.push_back(v3);

		vec3 normal(offset.z, 0, offset.x);
		normals_.push_back(normal);
		normals_.push_back(normal);
		normals_.push_back(normal);
		normals_.push_back(normal);

		vec3 c1(0.5, 0.5, 0.5);
		vec3 c2(1.0, 1.0, 1.0);
		colors_.push_back(c1);
		colors_.push_back(c1);
		colors_.push_back(c2);
		colors_.push_back(c2);

		int i = points_.size() - 4;
		indices_.push_back(i);
		indices_.push_back(i + 1);
		indices_.push_back(i + 2);

		indices_.push_back(i + 1);
		indices_.push_back(i + 3);
		indices_.push_back(i + 2);
	}
};

///////////////////////////////////////////////////
class MazeCamera : public Camera
{
public:
	bool      _moving;
	int       _currentNodeId;
	int       _nextNodeId;
	int       _currentDir;
	double    _startTime;
	float     _motionDuration;
	Ptr<Maze> _maze;

	MazeCamera(Ptr<Maze> maze)
		: _maze(maze),
		_moving(false),
		_currentNodeId(0),
		_nextNodeId(0),
		_currentDir(0),
		_startTime(0),
		_motionDuration(1.0)
	{}

	vec3 lerp(vec3 start, vec3 end, float u)
	{
		return vec3(start*(1 - u) + end*u);
	}

	void moveForward() {
		// dir = 0 => W, 1 => N, 2 => W, 3 => S
		if (_moving) return;
		// see if edge exists
		_nextNodeId = _maze->getNodes()[_currentNodeId].neighbors[_currentDir];
		if (_nextNodeId == -1) return;
		// start moving
		_moving = true;
		_startTime = getNow();
	}

	void turn(int leftOrRight) {
		// leftOrRight == 1 for left, -1 for right
		if (_moving) return;

		// not animated :-(
		_currentDir = (_currentDir + leftOrRight) % 4;
		if (_currentDir < 0) _currentDir += 4;
		// may not be an edge, so use direction itself
		vec3 lookDir;
		switch (_currentDir) {
		case 0: lookDir = vec3(1, 0, 0); break;
		case 1: lookDir = vec3(0, 0, -1); break;
		case 2: lookDir = vec3(-1, 0, 0); break;
		case 3: lookDir = vec3(0, 0, 1); break;
		}
		setLookat(position + lookDir);

	}
	void update() {
		if (!_moving) return;
		vec3 eyelevel(0, _maze->getHeight() / 2, 0);
		vec3 prevPos = _maze->getNodes()[_currentNodeId].position + eyelevel;
		vec3 nextPos = _maze->getNodes()[_nextNodeId].position + eyelevel;
		float u = min(1.0, (getNow() - _startTime) / _motionDuration);
		setPosition(lerp(prevPos, nextPos, u));

		if (u >= 1.0) {
			// you have reached your destination
			_moving = false;
			_currentNodeId = _nextNodeId;
			_nextNodeId = _maze->getNodes()[_currentNodeId].neighbors[_currentDir];
			turn(0);
		}
	}
};
///////////////////////////////////////////////////


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (mode == GOD_MODE) {
		godCamera.draw(root);
	}
	else {
		mazeCamera->draw(root);
	}
	glutSwapBuffers();
}

void timer(int delay)
{
	mazeCamera->update();
	root->update(0.033); // assumes called at 30Hz
	glutTimerFunc(delay, timer, delay);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	width = w;
	height = h;
	godCamera.setupPerspective(w, h);
	mazeCamera->setupPerspective(w, h);
}



void keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	case 'w':
		mazeCamera->moveForward(); break;
	case 'a':
		mazeCamera->turn(1); break;
	case 'd':
		mazeCamera->turn(-1); break;

	case 'g':
		mode = GOD_MODE; break;
	case 'm':
		mode = MAZE_MODE; break;

	case 'p':
		prim->setDrawingPrimitive(GL_POINTS);
		break;

	case 'l':
		prim->setDrawingPrimitive(GL_LINES);
		break;

	case 't':
		prim->setDrawingPrimitive(GL_TRIANGLES);
		break;

	case 27: /* ESC */
		exit(0);
		break;
	default:
		break;
	}
}

void
mouse(int button, int state, int x, int y)
{
	if (mode == GOD_MODE && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		godCamera.startMouse(x, height - y);
	}
}

void
motion(int x, int y)
{
	if (mode == GOD_MODE)
		godCamera.dragMouse(x, height - y);
}


void init(int argc, char **argv)
{

	// create a primitive using our subclass of Primitive. 
	Ptr<Maze> maze(new Maze(10, 10, 1.0, 0.5, 0.8));
	prim = maze;

	// create the MazeCamera
	mazeCamera = new MazeCamera(maze);

	// create a root Instance to contain this primitive
	Ptr<Instance> instance(new Instance());
	instance->setMatrix(mat4());
	instance->addChild(prim);

	// the lights are global for all objects in the scene
	RenderingEnvironment::getInstance().lightPosition = vec4(0, 0, 10, 1);
	RenderingEnvironment::getInstance().lightColor = vec4(1, 1, 1, 1);
	RenderingEnvironment::getInstance().addPointLight(vec3(8,8,8), vec3(0, 0, 0));
	
	// create a material to use
	Ptr<Material> mat(new Material);
	mat->ambient = vec4(0.2, 0.2, 0.2, 1.0);
	mat->diffuse = vec4(0.8, 0.8, 0.5, 1.0);
	mat->specular = vec4(1.0, 1.0, 1.0, 1.0);
	mat->shininess = 13.0;
	mat->program = mat->loadShaders("PhongShadingShadows");

	// attach the material
	instance->setMaterial(mat);

	// set the scene graph root 
	root = instance;

	// viewing mode
	mode = GOD_MODE;

	// enable godCamera trackball
	godCamera.enableTrackball(true);
	godCamera.setDistance(15.0);

	mazeCamera->setPosition(maze->getNodes()[0].position + vec3(0, maze->getHeight() / 2, 0));
	mazeCamera->setLookat(maze->getNodes()[1].position + vec3(0, maze->getHeight() / 2, 0));


	// misc OpenGL state
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	// render the shadow
	RenderingEnvironment::getInstance().getPointLight(0).updateShadow(root.get());

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(300, 300);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

#ifndef __APPLE__
	glewInit();
#endif
	init(argc, argv);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(30, timer, 30);
	glutMainLoop();
	return 0;
}


