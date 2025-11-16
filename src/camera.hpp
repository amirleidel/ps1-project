struct Camera {
    glm::vec3 position = glm::vec3(4.0f, 4.0f, 3.0f);

    float yaw   = -90.0f;   // pointing towards -Z by default
    float pitch = 0.0f;

    float speed       = 5.0f;   // units per second
    float sensitivity = 0.1f;

    glm::vec3 front   = glm::vec3(0,0,-1);
    glm::vec3 up      = glm::vec3(0,1,0);
    glm::vec3 right   = glm::vec3(1,0,0);

    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;
};


void handleMouse(Camera& cam, const SDL_Event& e) {
    if (e.type == SDL_MOUSEMOTION) {
        float xoffset = e.motion.xrel * cam.sensitivity;
        float yoffset = e.motion.yrel * cam.sensitivity;

        cam.yaw   += xoffset;
        cam.pitch -= yoffset;   // inverted y

        // limit pitch to prevent flipping
        if (cam.pitch > 89.0f) cam.pitch = 89.0f;
        if (cam.pitch < -89.0f) cam.pitch = -89.0f;

        // update direction vectors
        glm::vec3 dir;
        dir.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
        dir.y = sin(glm::radians(cam.pitch));
        dir.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
        cam.front = glm::normalize(dir);

        cam.right = glm::normalize(glm::cross(cam.front, glm::vec3(0,1,0)));
        cam.up    = glm::normalize(glm::cross(cam.right, cam.front));
    }
}

void handleKeyboard(Camera& cam, float dt) {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    float velocity = cam.speed * dt;

    if (state[SDL_SCANCODE_W])
        cam.position += cam.front * velocity;

    if (state[SDL_SCANCODE_S])
        cam.position -= cam.front * velocity;

    if (state[SDL_SCANCODE_A])
        cam.position -= cam.right * velocity;

    if (state[SDL_SCANCODE_D])
        cam.position += cam.right * velocity;

    // Optional: Up/down “flying”
    if (state[SDL_SCANCODE_SPACE])
        cam.position += cam.up * velocity;
    if (state[SDL_SCANCODE_LSHIFT])
        cam.position -= cam.up * velocity;
}

glm::mat4 getViewMatrix(const Camera& cam) {
    return glm::lookAt(
        cam.position,
        cam.position + cam.front,
        cam.up
    );
}
