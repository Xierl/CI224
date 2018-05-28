// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "common/objloader.hpp"

glm::mat4 getMVPMatrix(glm::mat4 modelTransform) {
	glm::mat4 Projection = glm::perspective(
		glm::radians(13.0f),						// The vertical Field of View, usually between 90°(extra wide) and 30°(quite zoomed in)
		4.0f / 3.0f,								// Aspect Ratio. Depends on the sizeof your window.
		0.1f,										// Near clipping plane. Keep as big aspossible, or you'll get precision issues.
		100.0f										// Far clipping plane. Keep as littleas possible.
	);
	glm::mat4 View = glm::lookAt(
		glm::vec3(10,10,10),						// 0,20,10 Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0),							// and looks at the origin
		glm::vec3(0, 1, 0)							// Head is up (set to 0,-1,0 to look upside - down)
		);
	//glm::mat4 oModel = glm::mat4(1.0f);				// keep an identity matrix so the geometry stays where it was placed originally
	glm::mat4 Model = modelTransform;
													// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model;		// Remember, matrix multiplication is the other way around
	return mvp;
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);					//4x anti-aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1080, 720, "Fahad and Nathan", NULL, NULL);	//Setting window size and window title
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1080 / 2, 720 / 2);						//width of the screen divided by two and height divided by two so the cursor is dead center
	glClearColor(0.5f, 0.0f, 0.5f, 0.5f);								//purple background

	// Buffers to help to describe data in a 3D scene
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs; // Won't be used at the moment.
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("arena.obj", vertices, uvs, normals);					//loading the floor

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	std::vector<glm::vec3> vertices1;
	std::vector<glm::vec2> uvs1; // Won't be used at the moment.
	std::vector<glm::vec3> normals1; // Won't be used at the moment.
	bool res1 = loadOBJ("player.obj", vertices1, uvs1, normals1);				//loading our player

	GLuint vertexbuffer1;
	glGenBuffers(1, &vertexbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(glm::vec3), &vertices1[0], GL_STATIC_DRAW);

	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	GLuint MatrixID1 = glGetUniformLocation(programID, "MVP");

	glm::mat4 mvp2 = getMVPMatrix(glm::translate(glm::vec3(0.0, 0.0, 0.0)));
	vec3 playerTransform = vec3(0.01f, 0.0f, 0.0f);

	do{
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear( GL_COLOR_BUFFER_BIT );

		glm::mat4 mvp1 = getMVPMatrix(glm::translate(glm::vec3(0.0, 0.0, 0.0)));
		glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &mvp1[0][0]);

		// Use our shader
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * sizeof(res));

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {					//down movement
			playerTransform = playerTransform + vec3(0.0f, 0.0f, 0.001);
				mvp2 = getMVPMatrix(translate(playerTransform));
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {					//right movement
			playerTransform = playerTransform + vec3(0.001f, 0.0f, 0.0f);
				mvp2 = getMVPMatrix(translate(playerTransform));
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {					//left movement
			playerTransform = playerTransform + vec3(-0.001f, 0.0f, 0.0f);
				mvp2 = getMVPMatrix(translate(playerTransform));
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {					//up movement
			playerTransform = playerTransform + vec3(0.0f, 0.0f, -0.001f);
				mvp2 = getMVPMatrix(translate(playerTransform));
		}


		glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &mvp2[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, vertices1.size() * sizeof(res1)); // Draw the triangles
		glDisableVertexAttribArray(0);	
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);						//to enable polygon mode and display all the vertices

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	glfwTerminate();// Close OpenGL window and terminate GLF

	return 0;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &vertexbuffer1);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
}

