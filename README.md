# Benternet ASCII art service

The service running on the benternet network from PXL will provide a quick and easy way to convert text into ascii art. Building a pretty CLI has never been easier. Additionally there is a service that you can subscribe to that will notify if new fonts are available. 

## Features

- Filesystem like CLI interface

- Multiple font options for customization

- Subscription-based changelog for font updates

## Usage

First connect to the benternet service. Benternet is a relay service that receives information and relays requests to each running service.  Create a socket to connect to the network

Benternet  requires two sockets to be created

`tcp://benternet.pxl-ea-ict.be:24041` Listen socket of benternet,  Here you can send requests that will be relayed to each service. 

`tcp://benternet.pxl-ea-ict.be:24042` Publish socket of benternet, Here responses will be sent to that have been broadcasted through the network.



On the server listen socket you are responsible for requesting the ASCII service. The ASCII server will expect a request with the following structure

`asciigenerator>[username]>[service]>[argument]>[parameters]`

The parameters are variable in length and depends on the service being called. Each parameter has to be provided in the right sequence and has to be seperated by the `>` delimiter.

### username

You can decide on an username. It is used to reply to requests and track user specific content. Keep in mind that the > character is forbidden to use in your username.

### service

the ASCII generator offers multiple services depending on the service different arguments are expected. If you request a service that does not exist the service will reply with a condensed help message.

## Services

### Help

The help service gives a detailed description about a service. If invalid arguments are provided to another service the same help message will be sent as if a help service was requested.

#### Expected arguments/parameters

Help expect the name of the service as an argument

### Fonts

The fonts service provides a list of available fonts on the service.

#### Expected arguments/parameters

Fonts expect no arguments or parameters

### generate

The generate service is the bread and butter of the service. It converts a string into ASCII art in different fonts

#### Expected arguments/parameters

generate expect a font, all fonts can found by running the fonts service or by checking the [Fonts]() section

The font is followed by optional font parameters. The last parameter is always the text that has to be converted to ASCII art

### settings

User can set specific settings for their username. It allowes for customized experiences.

#### Expected arguments/parameters

**Color**

The color argument followed by the parameter `true` or `false` will set the usage of color for multi-color fonts. By default this is set to false

## Changelog broadcasting

The service will also broadcast if new fonts have been added. Users can subscribe to the news by listening for messages that start with `asciigenerator>broadcast>newfont>`

After that it will anounce the name of the new font.

## Examples

*To do*
