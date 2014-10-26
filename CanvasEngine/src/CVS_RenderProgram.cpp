#include "CVS_RenderProgram.h"
#include "CVS_Precompiled.h"

CVS_Texture2D::CVS_Texture2D()
{
	glGenTextures(1, &texture);
}

bool CVS_Texture2D::loadFile(char* filepath)
{
	SDL_Surface* img = IMG_Load(filepath);

	if(img == NULL)
	{
		printf("Error loading image %s\n", filePath);
		return false;
	}

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int Mode = GL_RGB;

	printf("pixel format = %d\n", img->format->BitsPerPixel);

	if(img->format->BytesPerPixel == 4)
	{
		Mode = GL_RGBA;
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	this->w = img->w;
	this->h = img->h;

	return true;
}

void CVS_Texture2D::loadSDL_Surface(SDL_Surface* img)
{
	if(img == NULL)
		return;
	int Mode = GL_RGB;

	printf("pixel format = %d\n", img->format->BitsPerPixel);

	if(img->format->BytesPerPixel == 4)
	{
		Mode = GL_RGBA;
	}
	
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, Mode, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	this->w = img->w;
	this->h = img->h;
	this->filePath = filePath;
	this->type = Mode == GL_RGBA? CVS_TXT_RGBA:CVS_TXT_RGB;
}

void CVS_Texture2D::setActive(int loc)
{
	glActiveTexture(GL_TEXTURE0 + loc);
	activeLocation = loc;
}

void CVS_Texture2D::bindToLocation(int location)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(location, activeLocation);
}

GLuint compileProgram(char* vertex_file_path, char* fragment_file_path)
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
            VertexShaderCode += "\n" + Line;
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

bool CVS_RenderProgram::loadFile(char* vertpath, char* fragpath)
{
	this->ProgramName = compileProgram(vertpath, fragpath);

	int count;
	int acount;

	glUseProgram(ProgramName);

	glGetProgramiv(ProgramName, GL_ACTIVE_UNIFORMS, &count);

	printf("uniform count is%d\n", count);

	char* name = new char[200];
	for(int i = 0; i < count; ++i)
	{
		int arraySize = 0;
		GLenum type = GL_FALSE;
		GLsizei actualLength = 0;
		glGetActiveUniform(ProgramName, i, 200, &actualLength, &arraySize, &type, name);
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

int CVS_RenderProgram::getUniformHash(std::string name)
{
	for(int i = 0; i < uniforms.size(); ++i)
	{
		if(uniforms[i].name == name)
		{
			return uniforms[i].location;
		}
	}
	printf("Error, location not found, returning invalid uniform -1 %s\n", name.c_str());
	return -1;
}

void CVS_RenderProgram::bindVec4v(int hash, float* vector)
{
	glUniform4fv(hash, 1, vector);
}

void CVS_RenderProgram::bindMat4v(int hash, float* pointer)
{
	glUniformMatrix4fv(hash, 1, GL_FALSE, pointer);
}

void CVS_RenderProgram::setAsCurrentProgram()
{
	glUseProgram(ProgramName);


}