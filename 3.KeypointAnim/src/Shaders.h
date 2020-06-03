#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "Mesh.h"
#include "CamControl.h"

namespace Shaders {

	gl::GlslProgRef static GetDeformationShader() {
		return gl::GlslProg::create(gl::GlslProg::Format()
			.vertex(CI_GLSL(150,
				uniform mat4	ciModelViewProjection;
		uniform int		deformMode;
		uniform float	t;   //time, varies between 0..1
		uniform vec3    min; //vector containing the minima of the object
		uniform vec3	max; //contains the maximal values of the object
		const float M_PI = 3.14159;

		in vec3			ciPosition;			//The vertices of the mesh
		out vec4		worldPosition;		//The modified output vertice you must set

		vec4 taper(vec4 pos, float k)
		{
			vec4 taperedPos = pos;
            
            float s = (max.y - pos.y)/(max.y - min.y);
                        
            taperedPos.x = k * s * taperedPos.x + (1-k) * taperedPos.x;
            taperedPos.z = k * s * taperedPos.z + (1-k) * taperedPos.z;
            
			return taperedPos;
		}

		vec4 twist(vec4 pos, float k)
		{
			vec4 twistedPos = pos;
            
            float cosTerm = cos(k * pos.y * M_PI);
            float sinTerm = sin(k * pos.y * M_PI);
            
            twistedPos.x = pos.x * cosTerm - pos.z * sinTerm;
            twistedPos.z = pos.x * sinTerm + pos.z * cosTerm;
			
			return twistedPos;
		}


		float calcTheta(float y, float ymin, float ymax)
		{
			if (y < ymin) return 0.0;
			if (y > ymax) return ymax - ymin;
			return y - ymin;
		}

		vec4 bend(vec4 pos, float k)
		{
			float z0 = -1;			// The z coordinate of the "bend point"
			float ymin = (1-k)*2-0.75;	// = zmin in the book
			float ymax = 1;			// = zmax in the book
            float teta = pos.y - ymin;
            float r = z0 - pos.z;
			vec4 bentPos = pos;
			//todo: Task3 begin
            
            if (pos.y > ymax) {
                teta = ymax - ymin;
                bentPos.y = ymin - (r * sin(teta)) + (pos.y - ymax) * cos(teta);
                bentPos.z = z0 - (r * cos(teta)) + (pos.y - ymax) * sin(teta);
            }
            else if (pos.y >= ymin) {
                bentPos.y = ymin - (r * sin(teta));
                bentPos.z = z0 - (r * cos(teta));
            }

			return bentPos;
			//Task 3 end
		}
                            
        vec4 otherDeformation(vec4 pos, float k)
        {
            vec4 transformedPos = pos;
            
            float cosTerm = cos(k * pos.y * M_PI);
            float sinTerm = sin(k * pos.y * M_PI);
            
            transformedPos.x = pos.x * log(1 + pos.y) * k + pos.x * (1 -k);
//            transformedPos.z = pos.x * log(1 + k * pos.y);
        
            
            return transformedPos;
        }

		void main(void) {
			
			if (deformMode == 0)
				worldPosition = taper(vec4(ciPosition, 1), t);
			else if (deformMode == 1)
				worldPosition = twist(vec4(ciPosition, 1), t);
			else if (deformMode == 2)
				worldPosition = bend(vec4(ciPosition,1),t);
            else if (deformMode == 3)
                worldPosition = otherDeformation(vec4(ciPosition,1),t);
			gl_Position = ciModelViewProjection * worldPosition;
		}
		))
			.geometry(CI_GLSL(150,
				layout(triangles) in;
		layout(triangle_strip, max_vertices = 3)out;

		in vec4 worldPosition[];
		out vec3 normal;

		void main() {
			normal = normalize(cross(worldPosition[1].xyz - worldPosition[0].xyz, worldPosition[2].xyz - worldPosition[0].xyz));
			gl_Position = gl_in[0].gl_Position;
			EmitVertex();
			gl_Position = gl_in[1].gl_Position;
			EmitVertex();
			gl_Position = gl_in[2].gl_Position;
			EmitVertex();
			EndPrimitive();
		}
		))
			.fragment(CI_GLSL(150,
				out vec4			oColor;
		in vec3		normal;
		uniform vec3	    lightDir;

		void main(void) {
			oColor = vec4(vec3(1, 0.5, 0.25) * (max(dot(lightDir, normal), 0) + 0.2), 1);
		}
		)));
	}

	gl::GlslProgRef static GetFFDShader() {
		return gl::GlslProg::create(gl::GlslProg::Format()
			.vertex(CI_GLSL(150,
		uniform mat4	ciModelViewProjection;
		//The Control points describe the corners of a cube
		uniform vec3 controlPointsOrig[8];  //This array holds the corners of the cube and does not change (it is always (-1,-1,-1), (-1,1,-1)....)
		uniform vec3 controlPoints[8];      //This array holds the current position of the control points and will change every frame
		uniform mat4 mv;					//This matrix is only used to center the teapot

		//All positions are absolute, no need for any model->world transformations
		in vec3			ciPosition;			//The vertices of the mesh
		out vec4		worldPosition;		//The modified output vertice you must set

		
		vec3 transformPoint(vec3 p)
		{
			//todo: Task4
			//Assume that the Source cube always is -1..1 on each axis
			//Have a look at controlPointsOrig and controlPoints
			float dep[8];
			vec3 transformedPos = vec3(0, 0, 0);
                        
            for (int i = 0; i < 8; i++) {
                vec3 distance = - abs(p - controlPointsOrig[i]) / 2 + 1;
                dep[i] = distance.x * distance.y * distance.z;
                transformedPos += dep[i] * controlPoints[i];
            }
            
			return transformedPos;
			//Task4 end
		}

		void main(void) {
			vec3 pos = transformPoint((mv * vec4(ciPosition,1)).xyz);
			worldPosition = vec4(pos,1);
			gl_Position = ciModelViewProjection * worldPosition;
		}

		))
			.geometry(CI_GLSL(150,
				layout(triangles) in;
		layout(triangle_strip, max_vertices = 3)out;

		in vec4 worldPosition[];
		out vec3 normal;

		void main() {
			normal = normalize(cross(worldPosition[1].xyz - worldPosition[0].xyz, worldPosition[2].xyz - worldPosition[0].xyz));
			gl_Position = gl_in[0].gl_Position;
			EmitVertex();
			gl_Position = gl_in[1].gl_Position;
			EmitVertex();
			gl_Position = gl_in[2].gl_Position;
			EmitVertex();
			EndPrimitive();
		}
		))
			.fragment(CI_GLSL(150,
				out vec4			oColor;
		in vec3		normal;
		uniform vec3	    lightDir;

		void main(void) {
			oColor = vec4(vec3(1, 0.5, 0.25) * (max(dot(lightDir, normal), 0) + 0.2), 1);
		}
		)));
	}
};
