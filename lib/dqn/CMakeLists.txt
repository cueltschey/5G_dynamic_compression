file(GLOB DQN_SOURCE *.cpp)
add_library(dqn STATIC
  ${DQN_SOURCE}
)

target_link_libraries(dqn fmt "${TORCH_LIBRARIES}")
