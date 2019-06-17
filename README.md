# libmpdserver

Parser combinator library for [MPD][musicpd] client commands.

## Status

Currently primarily a toy project for experimenting with parser
combinators. Buggy, incomplete and unstable at the moment.

## Design Goals

This library attempts to follow some of the [langsec][langsec website]
principles outlined in [\[1\]][curing the vulnerable parsers]. This
include a strict input grammar enforced using a parser combinator and
performing full input recognition in the library before doing further
processing in the application.

## Compilation

Compile using:

	$ make

Run the test suite using:

	$ make check

Perform some sanity check with valgrind using:

	$ make valgrind

## License

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero
General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

[musicpd]: https://musicpd.org/
[langsec website]: http://langsec.org/
[curing the vulnerable parsers]: https://www.usenix.org/publications/login/spring2017/bratus
