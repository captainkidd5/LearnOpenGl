#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


int main() {
	//create two triangles, the one on top is oriented base-down, and the one below it is oriented base-up

	//triangle one oriented normally
	float triangleOneVerticies[] = {
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	//triangle two upside-down
	float triangeTwoVerticies[] = {
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f
	};

	//First, we create memory on the GPU to store the vertex data
	//This memory is managed by a Vertex Buffer Object (VBO). It can store a large number of verticies in GPU memory

	//VBOs are good because they can send large batches of data all at once without having to send one vertex at a time
	//Sending data from the CPU is kinda slow, so we want to do it all at once when possible

	//Once this data is in the graphics card's memory, the vertex shader has fast access to it



	//This will store the unique id of the Vertex Buffer Object when we 
	unsigned int VBO;

	//ask opengl to create it here
	glGenBuffers(1, &VBO);

	//There are many types of buffer objects in opengl. The buffer type specifically for vertex buffer objects is the GL_ARRAY_BUFFER.

	//We can bind to multiple buffers at once, given that they have different buffer types.
	//Let's bind it. Until we say otherwise, our VBO variable is now the active VBO being used by opengl on the GL_ARRAY_BUFFER target

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//This will copy the vertex data into the buffer's memory
	//type of buffer, size of the data in bytes, the actual data, and how the graphics card should manage the given data
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleOneVerticies), triangleOneVerticies, GL_STATIC_DRAW);


	//Now, open gl requires that we set up at least one vertex and one fragment shader.

		//VERTEX SHADER

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	//Each shader begins with the declaration of its version. 330 == OpenGl 3.3. We are also using core functionality.
	//each vertex has a 3D coordinate so we pass in a vec3. The location of the input variable will be at location 0.

	//Whatever we set as gl_Position will be the output of the shader. Input is a vec3, but the output must be a vec4, so just set
	//its "w" coordinate as 1 to cast it

	//The shader object also needs to be referenced by an id

	unsigned int vertexShader;

	//This creates the shader and stores its handle in the vertexShader variable
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Now, we attach the shader source code to the shader object
	//shader to compile, how many strings to use as source code, the actual source code, and null?
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

	//then we compile it
	glCompileShader(vertexShader);

	//then we can optionally do a check to see if compilation of the shader was successful

	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		//if no success, log it
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//the vertex shader is now compiled

	//FRAGMENT SHADER
	//The fragment shader calculates the color output of the pixels. In this case we will just make them orange-ish
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";

	//Every pixel will have the normalized rgba value of 1, .5, .2, and 1

	//create a handle for the fragment shader
	unsigned int fragmentShader;
	//create a fragment shader and return the handle
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);


	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		//if no success, log it
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Now we have compiled both the vertex shader and the fragment shader

	//Remember, the vertex shader determines where each pixel is drawn, fragment shader determines the color per pixel


	//The Shader Program:
	//This is the final linked version of multiple shaders combined. We must link the previous shader to this program in order to 
	//issue render calls


	//create a handle for the shader program
	unsigned int shaderProgram;
	//Then create the program and assign the handle
	shaderProgram = glCreateProgram();

	//attach the vertex shader to the program
	glAttachShader(shaderProgram, vertexShader);
	//attach the fragment shader to the program
	glAttachShader(shaderProgram, fragmentShader);


	//Get the result of shader program creation and store it in success
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Now we can tell open gl to use this specific program/shaders for future calls
	glUseProgram(shaderProgram);

	//we should also now delete the shader objects because we no longer need them
	glDeleteShader(vertexShader);

	glDeleteShader(fragmentShader);
	

	//At this point, we have sent the shader data to the gpu and instructed it how it should process the vertex data with said shaders
	//Opengl still does not know how to interpret the vertex data in memory, and how it should connect this data to the 
	//vertex shader's attributes

	//Position x, y, and z are stored as floats (4 bytes each). Each position has one of each.
	//Betwen each set of three there is no other room, they are tightly packed together
	//The very first value is th beginning of the buffer

	//now we can tell opengl how to interpret the vertex data!

	//in the vertex shader, we set location = 0. this means the location of the position of the vertex attribute should be at 0. We want to 
	//pass data to this vertex attribute so we pass in 0.
	//Next, we have the size of the vertex attribute (3 values)
	//next we specify the type of data (floats)
	//Next we specify if we should normalize the values (only relevant for ints and bytes).
	//Then say how much space is between consecutive vertex attributes. It's tightly packed, so there is no space between them. Each value is exactly 3 times the
	// size of a float away from the next
	//finally we say if there's an offset from the first value, which there isn't
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//The data which the attributes take their data from is determined by the VBO currently bound to the GL_ARRAY_BUFFER when calling glvertexattribpointer. 
	//We never unbound the previous VBO, so vertex attribute zero is now associated with that vertex data

	//VAO - Vertex Array Object
	//can be bound like a VBO, also allows 

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//the primitive type we want to draw, the starting index of the array, and the number of verticies to draw
	glDrawArrays(GL_TRIANGLES, 0, 3);



}