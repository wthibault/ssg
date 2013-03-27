// 
// ObjLoader.cpp -- modified for glm
//  modified from https://github.com/mortennobel/OpenGL_3_2_Utils
//  William.Thibault@csueastbay.edu
//
/*!
 * OpenGL 3.2 Utils - Extension to the Angel library (from the book Interactive Computer Graphics 6th ed
 * https://github.com/mortennobel/OpenGL_3_2_Utils
 *
 * New BSD License
 *
 * Copyright (c) 2011, Morten Nobel-Joergensen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ssg.h"
#include "ObjLoader.h"
using namespace glm;

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

const int DEBUG = 1;

void printDebug(vector<vec3> &positions, vector<int> &indices);
void printDebug(Material *m);
void printDebug(Texture *m);

vec3 toVec3(istringstream &iss){
	float x,y,z;
	iss >> x;
	iss >> y;
	iss >> z;
	return vec3(x,y,z);
}

vec2 toVec2(istringstream &iss){
	float x,y;
	iss >> x;
	iss >> y;
	return vec2(x,y);
}

struct TriangleIndex{
  // this is a vertex, represented as the indices into their 
  // respective arrays of positions, normals, and texture coords
	int position;
	int normal;
	int uv;

	bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if(start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	TriangleIndex(string p):position(-1),normal(-1),uv(-1) {
		// position/uv/normal
		replace(p, "//","/-1/");
		stringstream ss(p);
		char buffer[50];

		ss.getline(buffer,50, '/');
		position = atoi(buffer);
		if (ss.good()){
			ss.getline(buffer,50, '/');
			uv = atoi(buffer);
		}
		if (ss.good()){
			ss.getline(buffer,50, '/');
			normal = atoi(buffer);
		}
	}

	// needed to use TriangleIndex as key in map
	bool operator <(const TriangleIndex& Rhs) const {
		return (position < Rhs.position) ||
			(position == Rhs.position && normal < Rhs.normal) ||
			(position == Rhs.position && normal == Rhs.normal && uv < Rhs.uv);      
	}
};

struct TriangleString{
	TriangleIndex v0;
	TriangleIndex v1;
	TriangleIndex v2;

	TriangleString(string v0, string v1, string v2):v0(v0),v1(v1),v2(v2){
	}

	TriangleIndex get(int index){
		if (index == 0) {
			return v0;
		} else if (index == 1) {
			return v1;
		} 
		return v2;
	}
};


std::string
makeMtlFilename ( std::string mtlfile, std::string objfile )
{
  // pull off the path to the directory from the objfile
  size_t pos = objfile.rfind ('/');
  // tack the directory onto the materal "mtllib" filename
  if ( pos == std::string::npos ) {
    return mtlfile;
  }  else {
    std::string path;
    path = objfile.substr(0,pos);
    return path+"/"+mtlfile;
  }
}


bool loadMaterialLibrary ( string mtlfilename,
			   string objfilename,
			   map<string,Material*> &outMaterials,
			   vector<Texture *> &outDiffuseTextures,
			   vector<Texture *> &outBumpTextures )
{
  
  string filename = makeMtlFilename( mtlfilename, objfilename );
  string path = ssgPrependDataPath ( filename.c_str() );
  ifstream ifs ( path.c_str(), ifstream::in );
  if (!ifs) {
    std::cout << "can't open " << filename << std::endl;
    // create a default texture
    Material *m = new Material();
    outMaterials["dummy1"] = m;
    m->program = m->loadShaders("PhongShading");
    return false;
  }
  char buffer[512];
  while (ifs.good()) {
    ifs.getline(buffer,512);
    string line(buffer);
    istringstream iss(line);
    string token;
    vec3 color;
    Material *mat;
    iss >> token;
    if (token.compare("newmtl") ==0) {
      // create a new material and store in map
      mat = new Material;
      iss >> token;
      outMaterials[token] = mat;
    } else if (token.compare("Ka") == 0) {
      mat->ambient = vec4 ( toVec3 ( iss ), 1.0 );
    } else if (token.compare("Kd") == 0) {
      mat->diffuse = vec4 ( toVec3 ( iss ), 1.0);
    } else if (token.compare("Ks") == 0) {
      mat->specular = vec4 ( toVec3 ( iss ), 1.0);
    } else if (token.compare("Ns") == 0) {
      float shininess;
      iss >> mat->shininess;
    } else if (token.compare("map_Kd") == 0) {
      string filename,path;
      iss >> filename;
      if (filename[0] == '/') {
	mat->diffuseTexture = new Texture ( filename.c_str(), false, true, 0 ); // diffuse in unit 0
      } else {
	path = makeMtlFilename ( filename, objfilename );
	mat->diffuseTexture = new Texture ( path.c_str(), false, true, 0 ); // diffuse in unit 0
      }
      std::cout << "map_Kd from " << filename << std::endl;
    } else if (token.compare("map_Disp") == 0) {
      string filename,path;
      iss >> filename;
      if ( filename[0] == '/' ) { 
	mat->bumpTexture = new Texture ( filename.c_str(), false, true, 1 ); // bump in unit 1
      } else {
	path = makeMtlFilename ( filename, objfilename );
	mat->bumpTexture = new Texture ( path.c_str(), false, true, 1 ); // diffuse in unit 0
      }
    }
  }
  for ( map<string,Material*>::iterator i = outMaterials.begin(); 
	i != outMaterials.end(); 
	i++) {
    if ( i->second->diffuseTexture != 0 && i->second->bumpTexture != 0 )
      i->second->program = i->second->loadShaders ( "BumpMappedTexturedPhongShading" );
    else if ( i->second->diffuseTexture != 0 && i->second->bumpTexture == 0 )
      i->second->program = i->second->loadShaders ( "TexturedPhongShading" );
    else if ( i->second->diffuseTexture ==0 && i->second->bumpTexture != 0 )
      i->second->program = i->second->loadShaders ( "BumpMappedPhongShading" );
    else
      i->second->program = i->second->loadShaders ( "PhongShading" );
  }
}

bool loadObject(const char * filename, 
	vector<vec3> &outPositions, 
	vector<unsigned int> &outIndices,
	vector<vec3> &outNormal, 
	vector<vec2> &outUv,
	float scale)
{
	
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> uvs;


	vector<TriangleString> triangles;
	ifstream ifs ( filename , ifstream::in );
	char buffer[512];
	while (ifs.good()){
		ifs.getline(buffer,512);
		
		string line(buffer);
		istringstream iss(line);
		string token;
		iss >> token;
		if (token.compare("o")==0){
			// does not support multiple objects
		} else if (token.compare("g")==0){
		} else if (token.compare("mtllib")==0){
		} else if (token.compare("usemtl")==0){
			// does not support multiple materials
		} else if (token.compare("v")==0){
			positions.push_back( toVec3(iss));
		} else if (token.compare("vn")==0){
			normals.push_back( toVec3(iss));
		} else if (token.compare("vt")==0){
			uvs.push_back( toVec2(iss));
		} else if (token.compare("f")==0){
			vector<string> polygon;
			do {
				string index;
				iss >> index;
				if (index.length() > 0) {
					polygon.push_back(index);
				}
			} while (iss);

			// triangulate polygon (assumes convex )
			TriangleString triangle(polygon[0], polygon[1], polygon[2]);
			triangles.push_back(triangle);
			for (int i=3;i<polygon.size();i++){
				TriangleString triangle2(polygon[i-1], polygon[i], polygon[0]);
				triangles.push_back(triangle2);
			}
		}
	}
	ifs.close();


	map<TriangleIndex,int> cache;
	for (int i=0;i<triangles.size();i++){
		TriangleString triangleString = triangles[i];
		for (int j=0;j<3;j++){
			TriangleIndex index = triangleString.get(j);
			map<TriangleIndex,int>::iterator cachedIndex = cache.find(index);
			if (cachedIndex != cache.end()) {
				outIndices.push_back(cachedIndex->second);
			} else {
				int vertexIndex = outPositions.size();
				outPositions.push_back(positions[index.position-1] * scale);
				if (index.normal != -1){
					outNormal.push_back(normals[index.normal-1]);
				}
				if (index.uv != -1) {
					outUv.push_back(uvs[index.uv-1]);
				}
				outIndices.push_back(vertexIndex);
				cache[index] = vertexIndex;
			}
		}
	}
	//cout <<"Indices "<< outIndices.size() << endl;
	//cout <<"Positions "<< outPositions.size() << endl;
	//printDebug(outPositions, outIndices);
	return true;
}



Material *
makeDefaultMaterial()
{
  Material *m = new Material;
  m->ambient = vec4 ( 0.1, 0.1, 0.1, 1.0 );
  m->diffuse = vec4 ( 0.8, 0.8, 0.8, 1.0 );
  m->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  m->shininess = 200.0f;
  m->program = m->loadShaders("PhongShading");
  return m;
}



  struct Group {
    vector<TriangleString> triangles;
    Material *mat;
  };

//
// loadObjectGroups loads a .obj file containing materials and textures,
// creating a ModelNode that draws it.
bool loadObjectGroups ( const char * filename, 
			vector<vec3> &outPositions, 
			vector<vec3> &outNormal, 
			vector<vec2> &outUv,
			vector< vector<unsigned int> > &outIndices, // per group
			vector<Material *> &outMaterials, // per group
			float scale)
{
	
  vector<vec3> positions;
  vector<vec3> normals;
  vector<vec2> uvs;

  map<string,Group*> groups;
  string currentGroupName("");


  map<string,Material*> materials;
  vector<Texture*>      diffuseTextures;
  vector<Texture*>      bumpTextures;

  static int groupnum = 1;
  //  currentGroupName = "dummy1";
  //  groups[currentGroupName] = new Group;
  //  Material* defaultMaterial = makeDefaultMaterial();
  //  groups[currentGroupName]->mat = defaultMaterial;
  //  materials[currentGroupName] = defaultMaterial;
  //  outMaterials.push_back ( defaultMaterial );
  

  string path = ssgPrependDataPath ( filename );
  ifstream ifs ( path.c_str() , ifstream::in );
  char buffer[512];
  while (ifs.good()){
    ifs.getline(buffer,512);
		
    string line(buffer);
    istringstream iss(line);
    string token;
    iss >> token;
    if (token.compare("o")==0){
      // does not support multiple objects
    } else if (token.compare("g")==0){
      // for a new group of faces (eg with a diff material)
      // create a group
      iss >> currentGroupName;
      groups[currentGroupName] = new Group;
    } else if (token.compare("mtllib")==0){
      // read the .mtl file and create the Materials
      string mtlfile;
      iss >> mtlfile;
      loadMaterialLibrary ( mtlfile.c_str(),
			    filename,
			    materials,
			    diffuseTextures,
			    bumpTextures );
    } else if (token.compare("usemtl")==0){
      // create group if none exists, or if this is a "usemtl" line without a preceding "g" line
      if ( currentGroupName=="" || groups[currentGroupName]->triangles.size() ) {
	string name;
	ostringstream oss(name);
	//	static int groupnum = 1;
	oss << "dummy" << groupnum++;
	currentGroupName=oss.str();
	groups[currentGroupName] = new Group;
      }
      iss >> token;
      if (token=="usemtl") {
	std::cout << "null token" << std::endl;
	token = "dummy1";
      } 
      std::cout << "group is " << currentGroupName << std::endl;
      std::cout << "usemtl " << token << std::endl;
      std::cout << "materials[token] == " << materials[token] << std::endl;
      if ( materials[token] == 0 ) {
	materials[token] = makeDefaultMaterial();
      }
      printDebug ( materials[token] );
      groups[currentGroupName]->mat = materials[token];
      outMaterials.push_back(materials[token]);

    } else if (token.compare("v")==0){
      positions.push_back( toVec3(iss));
    } else if (token.compare("vn")==0){
      normals.push_back( toVec3(iss));
    } else if (token.compare("vt")==0){
      uvs.push_back( toVec2(iss));
    } else if (token.compare("f")==0){
      // assign face to current group
      vector<string> polygon;
      do {
	string index;
	iss >> index;
	if (index.length() > 0) {
	  polygon.push_back(index);
	}
      } while (iss);
      
      // oops no usemtl or mtllib or group seen
      //      if ( currentGroupName=="" || groups[currentGroupName]->triangles.size()==0 ) {
      if ( currentGroupName=="" ) {
	string name;
	ostringstream oss(name);
	//	static int groupnum = 1;
	oss << "dummy" << groupnum++;
	currentGroupName=oss.str();
	groups[currentGroupName] = new Group;
	outMaterials.push_back( groups[currentGroupName]->mat = makeDefaultMaterial() );
      }


      // triangulate polygon
      if ( polygon.size() >= 3 ) {
	TriangleString triangle(polygon[0], polygon[1], polygon[2]);
	groups[currentGroupName]->triangles.push_back(triangle);
	for (int i=3;i<polygon.size();i++){
	  TriangleString triangle2(polygon[i-1], polygon[i], polygon[0]);
	  groups[currentGroupName]->triangles.push_back(triangle2);
	}
      }
    }
  }
  ifs.close();
  
  // XXX
  // repeat this for each group.
  // return vectors must be made vectors of vectors (or maybe just the indices?)
  // for each group
  //  deref and pack the vertex positions, normal, and uv,
  //  pack the element array for each group separately
  map<TriangleIndex,int> cache;
  for ( map<string,Group*>::iterator g = groups.begin(); g != groups.end();g++ ) {
    // each group gets its own element array
    vector<unsigned int> groupIndices;
    for (int i=0;i<g->second->triangles.size();i++){
      TriangleString triangleString = g->second->triangles[i];
      for (int j=0;j<3;j++){
	TriangleIndex index = triangleString.get(j);
	map<TriangleIndex,int>::iterator cachedIndex = cache.find(index);
	if (cachedIndex != cache.end()) {
	  groupIndices.push_back(cachedIndex->second);
	} else {
	  int vertexIndex = outPositions.size();
	  outPositions.push_back(positions[index.position-1] * scale);
	  if (index.normal != -1){
	    outNormal.push_back(normals[index.normal-1]);
	  }
	  if (index.uv != -1) {
	    outUv.push_back(uvs[index.uv-1]);
	  }
	  groupIndices.push_back(vertexIndex);
	  cache[index] = vertexIndex;
	}
      }
    }
    // pack the indices for this group
    outIndices.push_back(groupIndices);
  }
  //cout <<"Indices "<< outIndices.size() << endl;
  //cout <<"Positions "<< outPositions.size() << endl;
  //printDebug(outPositions, outIndices);
  return true;
}


// std::ostream &operator<< (std::ostream &out, std::map<std::string,Group *> &groups) {
//   out << "{" ;
//   for ( std::map<std::string,Group *>::iterator g=groups.begin(); g != groups.end(); g++ ) {
//     out << g.triangles.size() << " vertices" << vec.y << " "<< vec.z 
// 	  }
//         << "}";

//     return out;
// }

void printDebug(vector<vec3> &positions, vector<int> &indices){
	for (int i=0;i<indices.size();i++){
	  //		cout << positions[indices[i]] <<" ";
		cout << positions[indices[i]].x <<",";
		cout << positions[indices[i]].y <<",";
		cout << positions[indices[i]].z <<" ";
		if ((i+1)%3==0){
			cout << endl;
		}
	}
}

void printDebug(Texture *t)
{
  cout << "textureId " << t->textureId;
  cout << "width " << t->width;
  cout << "height " << t->height;
  cout << endl;
}

void printDebug(Material *m){
  cout << "ambient " << m->ambient << endl;
  cout << "diffuse " << m->diffuse << endl;
  cout << "specular " << m->specular << endl;
  cout << "shininess " << m->shininess << endl;
  cout << "program " << m->program << endl;
  cout << "diffuse texture " << m->diffuseTexture << endl;
  if ( m->diffuseTexture ) 
    printDebug ( m->diffuseTexture );
  cout << "bump texture " << m->bumpTexture << endl;
  if ( m->bumpTexture ) 
    printDebug ( m->bumpTexture );
  
}


//
// ObjFilePrimitive
//

ObjFilePrimitive::ObjFilePrimitive ( const char *filename ) 
{

  //  std::vector<int> tmp_indices;
  bool okay = loadObjectGroups( filename,
				points_,
				normals_,
				texCoords_,
				groupIndices_,
				materials_,
				1.0 );
  if ( !okay ) {
    std::cout << "load error\n" ;
  } else {
    std::cout << "loaded " << filename << std::endl
	      << points_.size() << " points\n"
	      << normals_.size() << " normals\n"
	      << texCoords_.size() << " tex coords\n"
	      << groupIndices_.size() << " groups\n";
  }
  // // copy to unsigned ints. *sigh*
  // for (int i = 0; i < tmp_indices.size(); i++ )
  //   indices_.push_back( static_cast<unsigned int> (tmp_indices[i]) );

  drawingPrimitive_ = GL_TRIANGLES;

  generateAndLoadArrayBuffer();

  // generate the element buffers
  for ( int i = 0; i < groupIndices_.size(); i++ ) {
    groupElementArrayIds_.push_back(0);
    glGenBuffers(1, &groupElementArrayIds_[i] );
    int sizeofIndices = groupIndices_[i].size()*sizeof(unsigned int);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groupElementArrayIds_[i] );
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER, sizeofIndices, &(groupIndices_[i][0]), GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
}


void 
ObjFilePrimitive::draw ( glm::mat4 modelview, 
			 glm::mat4 projection,
			 Material *material) {

  // loop over all groups as they have different materials.
  for (int i = 0; i < groupIndices_.size(); i++ ) {

    // ignore given material, use our hard-won materials
    setupShader ( modelview, projection, materials_[i] );

    //
    // draw
    //
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, groupElementArrayIds_[i] );
    glDrawElements( drawingPrimitive_, groupIndices_[i].size(), GL_UNSIGNED_INT, 0  );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
    
    endShader();
  }
}
