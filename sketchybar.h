#pragma once

#include <mach/mach.h>
#include <mach/message.h>
#include <bootstrap.h>
#include <stdlib.h>

struct mach_message {
  mach_msg_header_t header;
  mach_msg_size_t msgh_descriptor_count;
  mach_msg_ool_descriptor_t descriptor;
};

struct mach_buffer {
  struct mach_message message;
  mach_msg_trailer_t trailer;
};

mach_port_t mach_get_bs_port() {
  mach_port_name_t task = mach_task_self();

  mach_port_t bs_port;
  if (task_get_special_port(task,
                            TASK_BOOTSTRAP_PORT,
                            &bs_port            ) != KERN_SUCCESS) {
    return 0;
  }

  mach_port_t port;
  if (bootstrap_look_up(bs_port,
                        "git.felix.sketchybar",
                        &port                  ) != KERN_SUCCESS) {
    return 0;
  }

  return port;
}

void mach_receive_message(mach_port_t port, struct mach_buffer* buffer, bool timeout) {
  *buffer = (struct mach_buffer) { 0 };
  mach_msg_return_t msg_return;
  if (timeout)
    msg_return = mach_msg(&buffer->message.header,
                                          MACH_RCV_MSG | MACH_RCV_TIMEOUT,
                                          0,
                                          sizeof(struct mach_buffer),
                                          port,
                                          100,
                                          MACH_PORT_NULL             );
  else 
    msg_return = mach_msg(&buffer->message.header,
                                          MACH_RCV_MSG,
                                          0,
                                          sizeof(struct mach_buffer),
                                          port,
                                          MACH_MSG_TIMEOUT_NONE,
                                          MACH_PORT_NULL             );

  if (msg_return != MACH_MSG_SUCCESS) {
    buffer->message.descriptor.address = NULL;
  }
}

char* mach_send_message(mach_port_t port, char* message, uint32_t len) {
  if (!message || !port) {
    return NULL;
  }

  mach_port_t response_port;
  mach_port_name_t task = mach_task_self();
  if (mach_port_allocate(task, MACH_PORT_RIGHT_RECEIVE,
                               &response_port          ) != KERN_SUCCESS) {
    return NULL;
  }

  if (mach_port_insert_right(task, response_port,
                                   response_port,
                                   MACH_MSG_TYPE_MAKE_SEND)!= KERN_SUCCESS) {
    return NULL;
  }

  struct mach_message msg = { 0 };
  msg.header.msgh_remote_port = port;
  msg.header.msgh_local_port = response_port;
  msg.header.msgh_id = response_port;
  msg.header.msgh_bits = MACH_MSGH_BITS_SET(MACH_MSG_TYPE_COPY_SEND,
                                            MACH_MSG_TYPE_MAKE_SEND_ONCE,
                                            0,
                                            MACH_MSGH_BITS_COMPLEX       );

  msg.header.msgh_size = sizeof(struct mach_message);
  msg.msgh_descriptor_count = 1;
  msg.descriptor.address = message;
  msg.descriptor.size = len * sizeof(char);
  msg.descriptor.copy = MACH_MSG_VIRTUAL_COPY;
  msg.descriptor.deallocate = false;
  msg.descriptor.type = MACH_MSG_OOL_DESCRIPTOR;

  mach_msg(&msg.header,
           MACH_SEND_MSG,
           sizeof(struct mach_message),
           0,
           MACH_PORT_NULL,
           MACH_MSG_TIMEOUT_NONE,
           MACH_PORT_NULL              );

  struct mach_buffer buffer = { 0 };
  mach_receive_message(response_port, &buffer, true);
  if (buffer.message.descriptor.address)
    return (char*)buffer.message.descriptor.address;
  mach_msg_destroy(&buffer.message.header);

  return NULL;
}

char* sketchybar(char* message) {
  uint32_t message_length = strlen(message) + 1;
  char formatted_message[message_length + 1];

  char quote = '\0';
  uint32_t caret = 0;
  for (int i = 0; i < message_length; ++i) {
    if (message[i] == '"' || message[i] == '\'') {
      if (quote == message[i]) quote = '\0';
      else quote = message[i];
      continue;
    }
    formatted_message[caret] = message[i];
    if (message[i] == ' ' && !quote) formatted_message[caret] = '\0';
    caret++;
  }

  formatted_message[++caret] = '\0';

  char* response = mach_send_message(mach_get_bs_port(),
                                     formatted_message,
                                     caret + 1          );

  if (response) return response;
  else return (char*)"";
}
