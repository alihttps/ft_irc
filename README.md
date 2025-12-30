# ft_irc - Internet Relay Chat Server

An IRC server implementation in C++98, compliant with RFC 1459/2812 protocols.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Compilation](#compilation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [IRC Commands](#irc-commands)
- [Channel Modes](#channel-modes)
- [Numeric Reply Codes](#numeric-reply-codes)
- [Classes](#classes)
- [Technical Details](#technical-details)

---

## Overview

ft_irc is a lightweight IRC server that allows multiple clients to connect, authenticate, join channels, and communicate with each other. The server uses non-blocking I/O with `poll()` to handle multiple connections simultaneously without threading.

---

## Features

- Multi-client support using `poll()` for non-blocking I/O
- Password-protected server access
- Channel creation and management
- Channel operator privileges
- Private messaging between users
- Channel modes (invite-only, topic restriction, password, user limit)
- RFC 1459/2812 compliant message format
- Compatible with standard IRC clients (irssi, HexChat, WeeChat, etc.)

---

## Requirements

- C++98 compatible compiler (g++, clang++)
- POSIX-compliant system (Linux, macOS)
- Make

---

## Compilation

```bash
make        # Build the server
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Rebuild from scratch
```

Compilation flags: `-Wall -Wextra -Werror -std=c++98`

---

## Usage

```bash
./ircserv <port> <password>
```

**Parameters:**
- `port` - The port number for incoming connections (1-65535)
- `password` - The server password required for client authentication

**Example:**
```bash
./ircserv 6667 mysecretpassword
```

**Connecting with a client:**
```bash
# Using netcat (for testing)
nc localhost 6667

# Using irssi
irssi -c localhost -p 6667 -w mysecretpassword

# Using HexChat
# Server: localhost/6667
# Password: mysecretpassword
```

---

## Project Structure

```
ft_irc/
├── Makefile
├── main.cpp
├── Includes/
│   ├── Server.hpp       # Server class declaration, IRC numeric codes
│   ├── Client.hpp       # Client class declaration, CommandType enum
│   └── Channel.hpp      # Channel class declaration
├── Server/
│   └── server.cpp       # Core server logic, connection handling
├── Client/
│   ├── Client.cpp       # Client implementation, utility functions
│   └── Commands/        # Command handlers (one per file)
│       ├── PASS.cpp     # Password authentication
│       ├── NICK.cpp     # Nickname registration
│       ├── USER.cpp     # User registration
│       ├── JOIN.cpp     # Join channel
│       ├── PART.cpp     # Leave channel
│       ├── PRIVMSG.cpp  # Private/channel messages
│       ├── KICK.cpp     # Kick user from channel
│       ├── INVITE.cpp   # Invite user to channel
│       ├── TOPIC.cpp    # View/set channel topic
│       ├── MODE.cpp     # Channel mode management
│       ├── QUIT.cpp     # Disconnect from server
│       └── PING.cpp     # Ping/pong keepalive
└── Channel/
    └── Channel.cpp      # Channel implementation
```

---

## IRC Commands

### Registration Commands

#### PASS
Authenticate with the server password.
```
PASS <password>
```
- Must be sent before NICK and USER
- Error 462: Already registered
- Error 464: Password incorrect

#### NICK
Set or change nickname.
```
NICK <nickname>
```
- Nickname rules: 1-9 characters, starts with letter or underscore, alphanumeric with underscore/hyphen
- Error 431: No nickname given
- Error 432: Erroneous nickname
- Error 433: Nickname already in use

#### USER
Complete registration with username and realname.
```
USER <username> 0 * :<realname>
```
- Error 461: Not enough parameters
- Error 462: Already registered

### Channel Commands

#### JOIN
Join one or more channels.
```
JOIN <channel>[,<channel>...] [<key>[,<key>...]]
```
- Creates channel if it doesn't exist (creator becomes operator)
- Channel names must start with `#` or `&`
- Error 403: No such channel
- Error 471: Channel is full (+l)
- Error 473: Invite only channel (+i)
- Error 475: Bad channel key (+k)

#### PART
Leave one or more channels.
```
PART <channel>[,<channel>...] [:<message>]
```
- Error 403: No such channel
- Error 442: You're not on that channel

#### TOPIC
View or set channel topic.
```
TOPIC <channel> [:<new topic>]
```
- Without topic: returns current topic
- With topic: sets new topic (if permitted)
- Error 442: You're not on that channel
- Error 482: You're not channel operator (when +t is set)
- Reply 331: No topic is set
- Reply 332: Current topic

#### KICK
Remove a user from a channel.
```
KICK <channel> <nickname> [:<reason>]
```
- Operators only
- Error 441: User not in channel
- Error 442: You're not on that channel
- Error 482: You're not channel operator

#### INVITE
Invite a user to a channel.
```
INVITE <nickname> <channel>
```
- Required for invite-only channels (+i)
- If +i is set, only operators can invite
- Error 401: No such nick
- Error 442: You're not on that channel
- Error 443: User already on channel
- Error 482: You're not channel operator

#### MODE
View or change channel modes.
```
MODE <channel> [<modes> [<parameters>]]
```
- Without modes: returns current mode string
- Operators only can change modes
- Error 482: You're not channel operator

### Message Commands

#### PRIVMSG
Send a message to a user or channel.
```
PRIVMSG <target> :<message>
```
- Target can be nickname or channel name
- Error 401: No such nick/channel
- Error 404: Cannot send to channel (not a member)
- Error 411: No recipient given
- Error 412: No text to send

### Connection Commands

#### QUIT
Disconnect from the server.
```
QUIT [:<message>]
```
- Notifies all channels the user was in
- Closes the connection

#### PING
Respond to server ping (keepalive).
```
PING <server>
```
- Server responds with PONG

---

## Channel Modes

| Mode | Parameter | Description |
|------|-----------|-------------|
| `+i` | None | Invite-only - users must be invited to join |
| `-i` | None | Remove invite-only restriction |
| `+t` | None | Topic restricted - only operators can change topic |
| `-t` | None | Anyone can change topic |
| `+k` | key | Set channel password/key |
| `-k` | None | Remove channel password |
| `+o` | nickname | Give operator privilege to user |
| `-o` | nickname | Remove operator privilege from user |
| `+l` | limit | Set user limit (max members) |
| `-l` | None | Remove user limit |

**Examples:**
```
MODE #channel +i                    # Set invite-only
MODE #channel +k secretpass         # Set channel key
MODE #channel +o username           # Make user an operator
MODE #channel +l 50                 # Limit to 50 users
MODE #channel +it                   # Set invite-only and topic restriction
MODE #channel -k                    # Remove channel key
MODE #channel +o-o user1 user2      # Give op to user1, remove from user2
```

---

## Numeric Reply Codes

### Success Replies (001-099)

| Code | Name | Description |
|------|------|-------------|
| 001 | RPL_WELCOME | Welcome message after registration |
| 002 | RPL_YOURHOST | Server host information |
| 003 | RPL_CREATED | Server creation date |
| 004 | RPL_MYINFO | Server name and version |

### Command Replies (300-399)

| Code | Name | Description |
|------|------|-------------|
| 324 | RPL_CHANNELMODEIS | Current channel modes |
| 331 | RPL_NOTOPIC | No topic is set |
| 332 | RPL_TOPIC | Channel topic |
| 341 | RPL_INVITING | Invitation sent successfully |
| 353 | RPL_NAMREPLY | List of users in channel |
| 366 | RPL_ENDOFNAMES | End of /NAMES list |

### Error Replies (400-599)

| Code | Name | Description |
|------|------|-------------|
| 401 | ERR_NOSUCHNICK | No such nick/channel |
| 403 | ERR_NOSUCHCHANNEL | No such channel |
| 404 | ERR_CANNOTSENDTOCHAN | Cannot send to channel |
| 411 | ERR_NORECIPIENT | No recipient given |
| 412 | ERR_NOTEXTTOSEND | No text to send |
| 421 | ERR_UNKNOWNCOMMAND | Unknown command |
| 431 | ERR_NONICKNAMEGIVEN | No nickname given |
| 432 | ERR_ERRONEUSNICKNAME | Erroneous nickname |
| 433 | ERR_NICKNAMEINUSE | Nickname is already in use |
| 441 | ERR_USERNOTINCHANNEL | User not in channel |
| 442 | ERR_NOTONCHANNEL | You're not on that channel |
| 443 | ERR_USERONCHANNEL | User is already on channel |
| 451 | ERR_NOTREGISTERED | You have not registered |
| 461 | ERR_NEEDMOREPARAMS | Not enough parameters |
| 462 | ERR_ALREADYREGISTRED | You may not reregister |
| 464 | ERR_PASSWDMISMATCH | Password incorrect |
| 471 | ERR_CHANNELISFULL | Channel is full |
| 473 | ERR_INVITEONLYCHAN | Invite only channel |
| 475 | ERR_BADCHANNELKEY | Bad channel key |
| 482 | ERR_CHANOPRIVSNEEDED | You're not channel operator |

---

## Classes

### Server (Singleton)

The main server class handling all connections and command routing.

**Key Members:**
- `_listener` - Listening socket file descriptor
- `_pfds` - Vector of pollfd structures for poll()
- `_clients` - Map of fd to Client objects
- `_channels` - Map of channel name to Channel objects
- `_password` - Server password
- `_port` - Server port

**Key Methods:**
- `getInstance()` - Get singleton instance
- `run(port, password)` - Start the server
- `stop()` - Stop the server
- `handleNewConnection()` - Accept new client connections
- `handleClientData(fd)` - Process incoming data from client
- `routeCommand(fd, line)` - Parse and route commands
- `removeClient(fd)` - Disconnect and clean up client
- `getClientFdByNick(nickname)` - Find client by nickname
- `checkRegistration(fd)` - Check if client can complete registration
- `sendWelcome(fd)` - Send welcome messages
- Command handlers: `cmdPass`, `cmdNick`, `cmdUser`, `cmdJoin`, `cmdPart`, `cmdPrivmsg`, `cmdKick`, `cmdInvite`, `cmdTopic`, `cmdMode`, `cmdQuit`, `cmdPing`

### Client

Represents a connected IRC client.

**Key Members:**
- `_fd` - Client socket file descriptor
- `_buffer` - Incoming data buffer
- `_nickname` - Client nickname
- `_username` - Client username
- `_realname` - Client realname
- `_hostname` - Client IP address
- `_channels` - Set of joined channels
- `_passOk` - Password verification status
- `_nickSet` - Nickname set status
- `_userSet` - User info set status
- `_registered` - Full registration status

**Key Methods:**
- `getPrefix()` - Returns nick!user@host format
- `appendToBuffer(data)` - Append received data
- `extractLine(line)` - Extract complete line from buffer
- `sendMessage(message)` - Send message to client
- `sendReply(server, code, message)` - Send numeric reply
- `canRegister()` - Check if all registration conditions met
- `tryRegister()` - Attempt to complete registration
- `addChannel(name)` / `removeChannel(name)` - Manage channel membership

**Static Utility Methods:**
- `isValidNickname(nickname)` - Validate nickname format
- `split(str, delim)` - Split string by delimiter
- `parseIRCLine(line, command, params)` - Parse IRC protocol line
- `getCommandType(command)` - Convert command string to CommandType enum
- `requiresRegistration(type)` - Check if command requires registration

### Channel

Represents an IRC channel.

**Key Members:**
- `_name` - Channel name
- `_topic` - Channel topic
- `_key` - Channel password
- `_members` - Set of member file descriptors
- `_operators` - Set of operator file descriptors
- `_inviteList` - Set of invited nicknames
- `_inviteOnly` - Invite-only mode flag
- `_topicRestricted` - Topic restriction flag
- `_userLimit` - Maximum members (0 = unlimited)

**Key Methods:**
- `addMember(fd)` / `removeMember(fd)` - Manage members
- `isMember(fd)` - Check membership
- `addOperator(fd)` / `removeOperator(fd)` - Manage operators
- `isOperator(fd)` - Check operator status
- `addInvite(nickname)` / `removeInvite(nickname)` - Manage invites
- `isInvited(nickname)` - Check if user is invited
- `broadcast(message, excludeFd)` - Send message to all members
- `getModeString()` - Get current modes as string

---

## Technical Details

### Non-Blocking I/O

The server uses `fcntl(fd, F_SETFL, O_NONBLOCK)` to set all sockets to non-blocking mode. This allows the single-threaded server to handle multiple connections without blocking.

### poll() Event Loop

A single `poll()` call monitors all file descriptors:
- Listening socket for new connections
- Client sockets for incoming data

Events are processed in the order returned by poll().

### IRC Protocol

Messages follow the IRC protocol format:
```
[:prefix] COMMAND [params] [:trailing]
```

Lines are terminated with `\r\n` (CRLF).

### Registration Flow

1. Client connects
2. Client sends `PASS <password>`
3. Client sends `NICK <nickname>`
4. Client sends `USER <username> 0 * :<realname>`
5. Server sends welcome messages (001-004)
6. Client is now registered and can use all commands

PASS must be sent before completing registration, but NICK and USER can be sent in any order.

### Channel Creation

When a client joins a non-existent channel, the channel is created and the client becomes its first operator. Empty channels are automatically removed.

---

## Author

42 School Project - ft_irc

---

## License

This project is part of the 42 School curriculum.
