#!/usr/bin/env escript
% -------------------------------------------------------------------------
% "THE BEER-WARE LICENSE" (Revision 42):
% <datafl4sh@toxicnet.eu> wrote this file. As long as you retain this
% notice you can do whatever you want with this stuff. If we meet some
% day, and you think this stuff is worth it, you can buy me a beer in
% return. datafl4sh.
% -------------------------------------------------------------------------
%%! -smp enable -sname seaserver debug verbose

-import(seatalkng, [start/0]).

main(_) ->
    seatalkng:start().
