#include "Application.h"

#include <ctime>
#include <cstdlib>


namespace jq {

std::string Application::ReadShaderSource(std::string const &fname) {
  std::ifstream ifs(fname.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
  std::ifstream::pos_type file_size = ifs.tellg();

  ifs.seekg(0, std::ios::beg);

  std::vector<char> bytes(file_size);
  ifs.read(bytes.data(), file_size);

  return std::string(bytes.data(), file_size);
}

// Compile a shader
GLuint Application::LoadAndCompileShader(std::string const &fname, GLenum shaderType) {
  // Load a shader from an external file
  auto buffer = ReadShaderSource(fname);
  const char *src = &buffer[0];
  std::cout << buffer << endl;

  // Compile the shader
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);

  // Check the result of the compilation
  GLint test;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
  if(!test) {
    std::cerr << "Shader compilation failed with this message:" << std::endl;
    std::vector<char> compilation_log(512);
    glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
    std::cerr << &compilation_log[0] << std::endl;
    glfwTerminate();
    exit(-1);
  }
  return shader;
}

// Create a program from two shaders
GLuint Application::CreateShaderProgram(const char *path_vert_shader, const char *path_frag_shader) {
  // Load and compile the vertex and fragment shaders
  GLuint vertexShader = LoadAndCompileShader(path_vert_shader, GL_VERTEX_SHADER);
  GLuint fragmentShader = LoadAndCompileShader(path_frag_shader, GL_FRAGMENT_SHADER);
 
  // Attach the above shader to a program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
 
  // Flag the shaders for deletion
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
 
  // Link and use the program
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);
 
  return shaderProgram;
}

void Application::OpenGLKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void Application::OpenGLResizeCallback(GLFWwindow* window, int new_width, int new_height) {
  //glViewport(0, 0, m_window_width = new_width, m_window_height = new_height);
  glViewport(0, 0, new_width, new_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, new_width, new_height, 0.0, 0.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
}


/* data for the GL debug callback */
struct DebugUserParams {
  bool ShowNotifications = true;
} debuguserparams;

static int window_width = 800;
static int window_height = 600;

int Application::Init() {
  /* Initialize the library */
  if (!glfwInit()) {
    printf("Failed to initialize glfw.\n");
    return false;
  }

  /* Create a windowed mode window and its OpenGL context */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // TODO: Check which is prefered, COMPAT or CORE
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, m_bDebugcontext ? GLFW_TRUE : GLFW_FALSE);
  m_pMonitor = m_bFullscreen ? glfwGetPrimaryMonitor() : NULL;

  m_iWindowWidth = window_width;
  m_iWindowHeight = window_height;
  assert(m_iWindowWidth > 0);
  assert(m_iWindowHeight > 0);

  m_pWindow = glfwCreateWindow(m_iWindowWidth, m_iWindowHeight, "floating window", m_pMonitor, NULL);
  if (!m_pWindow) {
    glfwTerminate();
    printf("Failed to create glfw window.\n");
    return false;
  }
  /* Make the window's context current */
  glfwMakeContextCurrent(m_pWindow);

  /* Initialize GLEW */
  glewExperimental = GL_TRUE; http://glew.sourceforge.net/basic.html
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cout << "GLEW initialisation error: " << glewGetErrorString(err) << std::endl;
    glfwTerminate();
    return false;
  }
  std::cout << "GLEW okay - using version: " << glewGetString(GLEW_VERSION) << std::endl;

  /* Register debug callback if debug context is available */
  GLint contextflags = 0;
  glGetIntegerv(GL_CONTEXT_FLAGS, &contextflags);
  if (contextflags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGLDebugCallback, &debuguserparams);
  }

  //glm::ortho(-((float)window_width / (float)window_height), (float)window_width / (float)window_height, -1.0f, 1.0f, -1.0f, 1.0f);

  // Create VAO
  glGenVertexArrays(1, &m_vertexArrayID);
  glBindVertexArray(m_vertexArrayID);

  glEnable(GL_PROGRAM_POINT_SIZE);
  //glEnable(GL_PROGRAM_COLO);

  //NOTE: Wireframe
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Register key callback
  glfwSetKeyCallback(m_pWindow, OpenGLKeyCallback);

  m_GUI.Init(m_pWindow);
  return(true);
};


//std::vector<std::pair<float, float>>
void generateRandomPoints(float points[],int const n ) {
  std::srand(time(NULL));

  int x, y;
  bool xsign, ysign; // true if negative
  //TODO: Make true floating numbers
  for(int i=0; i< n-1; i+=2) {

    std::cout << x << ", " << y << std::endl;
    x = rand()%100;
    y = rand()%100;
     xsign = rand()%2;
     ysign = rand()%2;

     if(xsign) x *= -1;
     if(ysign) y *= -1;

     points[i] = x;
     points[i+1] = y;
     std::cout << x << ", " << y << std::endl;
   }
};

int Application::Exec(int argc, char* argv[]) {

  //Test test(VertexArrayID);
  //OpenGLTestNVX();
  if(Init()){

    // An array of 3 vectors which represents 3 vertices
#if 0
    static const GLfloat g_vertex_buffer_data[] = {
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      0.0f,  1.0f, 0.0f,
    };
#else
    static std::size_t const size = 8 ;
    float g_vertex_buffer_data[size];
    generateRandomPoints(g_vertex_buffer_data, size );
#endif


    // Generate 1 buffer, put the resulting identifier in m_vertexBuffer
    glGenBuffers(1, &m_vertexBuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


    CreateShaderProgram("shader.vert", "fragment_shader.txt");


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(m_pWindow))
    {
      // Clear
      static ImVec4 const clear_color = ImColor(114, 144, 154);
      glfwGetFramebufferSize(m_pWindow, &m_iWindowWidth, &m_iWindowHeight);
      glViewport(0, 0, m_iWindowWidth, m_iWindowHeight);
      /* clear framebuffer */
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);

      /* Render here */
      //Render();
      glBindVertexArray(m_vertexArrayID);


      // 1st attribute buffer : vertices
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
      glVertexAttribPointer(
          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
          3,                  // size
          GL_FLOAT,           // type
          GL_FALSE,           // normalized?
          0,                  // stride
          (void*)0            // array buffer offset
                            );

#if 0 // Draw points
      glDrawArrays(GL_POINTS, 0, 3);
#else // Draw a triangle
      glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
      //glDrawArrays(GL_TRIANGLE_STRIP, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
#endif

      glDisableVertexAttribArray(0);

      /* Swap front and back buffers */
      glfwSwapBuffers(m_pWindow);

      /* Poll for and process events */
      glfwPollEvents();
    }

  } else {
    return EXIT_FAILURE;
  }

  CHECK_GL_ERROR;
  CleanUp();
  glfwTerminate();

  return EXIT_SUCCESS;
}



void Application::Render() {

  m_GUI.Render();

  /* check for any error */
  CHECK_GL_ERROR;
}


void Application::CleanUp()
{
  ImGui_ImplGlfwGL3_Shutdown();
  // destroy all objects
  glDeleteBuffers(1, &m_vertexArrayID);
  glDeleteBuffers(1, &m_vertexBuffer);

  CHECK_GL_ERROR;
}

/* variables */
GLuint vertexbuffer = 0;
GLuint texture = 0;
GLuint VertexArrayID = 0;


/* typical vertex format */
struct Vertex
{
  glm::vec3 Position;
  glm::vec2 TexCoord;
  glm::vec3 Normal;
  glm::vec3 Tangent;
  glm::vec3 Bitangent;
  glm::uvec4 NodeIndices;
  glm::vec4 NodeWeights;
};

/* typical texel format */
struct Texel
{
  GLubyte R = 0;
  GLubyte G = 0;
  GLubyte B = 0;
  GLubyte A = 0;
};


bool Application::OpenGLTestNVX()
{
  /* check if GL_NVX_gpu_memory_info is supported */
  GLint extensioncount = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &extensioncount);

  bool extension_available = false;
  for (int i = 0; i < extensioncount; i++)
  {
    std::string extension((const char*)glGetStringi(GL_EXTENSIONS, i));
    if (extension == "GL_NVX_gpu_memory_info")
    {
      extension_available = true;
      break;
    }
  }

  if (!extension_available)
  {
    std::cout << "GL_NVX_gpu_memory_info not supported" << std::endl;
    return false;
  }



  /*
    --- quote from http://developer.download.nvidia.com/opengl/specs/GL_NVX_gpu_memory_info.txt ---
    These will return the memory status of the context's associated GPU memory.
    The status returned is not intended as an exact measurement of the
    system's current status.  It will provide a approximate indicator of
    the overall GPU memory utilization so that an application can determine
    when the resource usage is approaching the full capacity of the GPU memory
    and it may need to adjust its usage pattern to avoid performance limiting
    swapping.  Each query returns a integer where the values have the
    following meanings:
    GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX
    - dedicated video memory, total size (in kb) of the GPU memory
    GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
    - total available memory, total size (in Kb) of the memory
    available for allocations
    GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX
    - current available dedicated video memory (in kb),
    currently unused GPU memory
    GPU_MEMORY_INFO_EVICTION_COUNT_NVX
    - count of total evictions seen by system
    GPU_MEMORY_INFO_EVICTED_MEMORY_NVX
    - size of total video memory evicted (in kb)
  */


  /* query current GPU memory */
  struct
  {
    /* each in kB */
    GLint mem_info_dedicated_vidmem = 0;
    GLint mem_info_total_available_memory = 0;
    GLint mem_info_current_available_memory = 0;
    GLint mem_info_eviction_count = 0;
    GLint mem_info_evicted_memory = 0;
  } memory_before, memory_after;


  glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX,            &memory_before.mem_info_dedicated_vidmem);
  glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,    &memory_before.mem_info_total_available_memory);
  glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,    &memory_before.mem_info_current_available_memory);
  glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX,            &memory_before.mem_info_eviction_count);
  glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX,            &memory_before.mem_info_evicted_memory);


  // create all objects
  glCreateBuffers(1, &vertexbuffer);
  glCreateTextures(GL_TEXTURE_2D, 1, &texture);

  /* setup vertex buffer (just 1 million vertices) */
  std::vector<Vertex> vertices(1000000);
  glNamedBufferData(vertexbuffer, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  /* setup texture (4096 x 4096 rgba8) */
  std::vector<Texel> texels(4096 * 4096);
  glTextureStorage2D(texture, 1, GL_RGBA8, 4096, 4096);
  glTextureSubImage2D(texture, 0, 0, 0, 4096, 4096, GL_RGBA, GL_UNSIGNED_BYTE, texels.data());
  glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


  glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX,            &memory_after.mem_info_dedicated_vidmem);
  glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,    &memory_after.mem_info_total_available_memory);
  glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,    &memory_after.mem_info_current_available_memory);
  glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX,            &memory_after.mem_info_eviction_count);
  glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX,            &memory_after.mem_info_evicted_memory);


  /* what we expect: */
  GLint mem_consumption_expected = (sizeof(Vertex) * 1000000 + 4096 * 4096 * 4) / 1024;


  cout
      << "BEFORE creation of resources:" << endl
      << "mem_info_dedicated_vidmem: \t"            << memory_before.mem_info_dedicated_vidmem << " kB" << endl
      << "mem_info_total_available_memory: \t"    << memory_before.mem_info_total_available_memory << " kB" << endl
      << "mem_info_current_available_memory: \t"    << memory_before.mem_info_current_available_memory << " kB" << endl
      << "mem_info_eviction_count: \t"            << memory_before.mem_info_eviction_count << " kB" << endl
      << "mem_info_evicted_memory: \t"            << memory_before.mem_info_evicted_memory << " kB" << endl
      << "AFTER creation of resources:" << endl
      << "mem_info_dedicated_vidmem: \t"            << memory_after.mem_info_dedicated_vidmem << " kB" << endl
      << "mem_info_total_available_memory: \t"    << memory_after.mem_info_total_available_memory << " kB" << endl
      << "mem_info_current_available_memory: \t"    << memory_after.mem_info_current_available_memory << " kB" << endl
      << "mem_info_eviction_count: \t"            << memory_after.mem_info_eviction_count << " kB" << endl
      << "mem_info_evicted_memory: \t"            << memory_after.mem_info_evicted_memory << " kB" << endl
      << "EXPECTED:" << endl
      << "mem_consumption_expected: \t"            << mem_consumption_expected << " kB" << endl
      << "what the extension gives us: \t"
      << (memory_before.mem_info_current_available_memory - memory_after.mem_info_current_available_memory) << " kB" << endl
      << endl;


  CHECK_GL_ERROR;


  return true;
}


void Application::__CheckForGLError(const char* filename, int line)
{
  for (GLenum error; (error = glGetError()) != GL_NO_ERROR;)
  {
    cout << (char)7 << "OpenGL Error:\t" << filename << "  line " << line << "\n\t";
    if (error == GL_INVALID_ENUM)
      cout << "GL_INVALID_ENUM";
    if (error == GL_INVALID_VALUE)
      cout << "GL_INVALID_VALUE";
    if (error == GL_INVALID_OPERATION)
      cout << "GL_INVALID_OPERATION";
    if (error == GL_STACK_OVERFLOW)
      cout << "GL_STACK_OVERFLOW";
    if (error == GL_STACK_UNDERFLOW)
      cout << "GL_STACK_UNDERFLOW";
    if (error == GL_OUT_OF_MEMORY)
      cout << "GL_OUT_OF_MEMORY";
    if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
      cout << "GL_INVALID_FRAMEBUFFER_OPERATION";
    if (error == GL_CONTEXT_LOST)
      cout << "GL_CONTEXT_LOST";
    cout << endl << endl;
    std::cin.get();
  }
}


/* debug */
void APIENTRY Application::OpenGLDebugCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userparam)
{
  /* debug user params */
  DebugUserParams* params = (DebugUserParams*)userparam;

  /* filter out unnecessary warnings */
  if (!params->ShowNotifications)
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
      return;

  /* source */
  std::string str_source;
  if (source == GL_DEBUG_SOURCE_API) str_source = "API";
  if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM) str_source = "Window System";
  if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) str_source = "Shader Compiler";
  if (source == GL_DEBUG_SOURCE_THIRD_PARTY) str_source = "Third Party";
  if (source == GL_DEBUG_SOURCE_APPLICATION) str_source = "Application";
  if (source == GL_DEBUG_SOURCE_OTHER) str_source = "Other";

  /* type */
  std::string str_type;
  if (type == GL_DEBUG_TYPE_ERROR) str_type = "Error";
  if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) str_type = "Deprecated Behavior";
  if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) str_type = "Undefined Behavior";
  if (type == GL_DEBUG_TYPE_PORTABILITY) str_type = "Portability";
  if (type == GL_DEBUG_TYPE_PERFORMANCE) str_type = "Performance";
  if (type == GL_DEBUG_TYPE_MARKER) str_type = "Marker";
  if (type == GL_DEBUG_TYPE_PUSH_GROUP) str_type = "Push Group";
  if (type == GL_DEBUG_TYPE_POP_GROUP) str_type = "Pop Group";
  if (type == GL_DEBUG_TYPE_OTHER) str_type = "Other";

  /* severity */
  std::string str_severity;
  if (severity == GL_DEBUG_SEVERITY_HIGH) str_severity = "High";
  if (severity == GL_DEBUG_SEVERITY_MEDIUM) str_severity = "Medium";
  if (severity == GL_DEBUG_SEVERITY_LOW) str_severity = "Low";
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) str_severity = "Notification";

  /* print message */
  cout << "OpenGL Debug Message:" << endl;
  cout << "----------------------------------" << endl;
  cout << "ID: \t\t" << id << endl;
  cout << "Source: \t" << str_source << endl;
  cout << "Type: \t\t" << str_type << endl;
  cout << "Severity: \t" << str_severity << endl;
  cout << "Message: \t" << message << endl;
  cout << "----------------------------------" << endl << endl;
}

} // namespace jq