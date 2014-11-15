#include "CVS_RenderProgram.h"

CVS_Texture::CVS_Texture(UINT target) :target(target)
{
	glGenTextures(1, &texture);
}

bool CVS_Texture::loadData(UINT flags, UINT format, UINT type, int width, int height, int mipmap, void* data)
{
	Bind();
	glTexImage2D(target, mipmap, format, width, height, 0, flags, GL_FLOAT, data);
	return true;
}

bool CVS_Texture::loadFile(char* filepath)
{
	return true;
}

void CVS_Texture::Bind()
{
	glBindTexture(target, texture);
}

GLuint CompileShader(char* vertex_file_path, char* fragment_file_path)
{
	// Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
		{
            VertexShaderCode += "\n" + Line;
		}
        VertexShaderStream.close();
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
 
    GLint Result = GL_FALSE;
    int InfoLogLength;
 
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
 
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

	return ProgramID;
	glUseProgram(0);
}


CVS_RenderProgram::CVS_RenderProgram()
{
}


bool CVS_RenderProgram::loadFile(char* vertex_file_path, char* fragment_file_path)
{
	vertPath = std::string(vertex_file_path);
	fragPath = std::string(fragment_file_path);

	programID = CompileShader(vertex_file_path, fragment_file_path);

	GLuint uniformNumbers;

	int count;
	int acount;

	glUseProgram(programID);

	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &count);

	printf("uniform count is%d\n", count);

	char* name = new char[200];
	for(int i = 0; i < count; ++i)
	{
		int arraySize = 0;
		GLenum type = GL_FALSE;
		GLsizei actualLength = 0;
		glGetActiveUniform(programID, i, 200, &actualLength, &arraySize, &type, name);
		CVS_UniformLocation loc = {std::string(name), type, i};
		printf("New uniform found %s\n", loc.name.c_str());
		if(type == GL_SAMPLER_2D)
		{
			printf("And its a texture!\n");
		}
		uniforms.push_back(loc);
	}

	return true;
}

GLint CVS_RenderProgram::getUniformHash(std::string uniform)
{
	int ret = glGetUniformLocation(this->programID, uniform.c_str());
	if (ret == -1)
		printf("Error value does not exist\n");
	return ret;
}

void CVS_RenderProgram::setAsCurrentProgram()
{
	glUseProgram(programID);
}
