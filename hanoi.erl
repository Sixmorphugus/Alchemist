-module(hanoi).

% Exported functions
-export([create_towers/1]).
-export([display_towers/1]).
-export([move/3]).
-export([count_disks/1]).
-export([solve/1]).

% Private, creates the first tower with N rounds.
create_tower(0) ->
	[];
create_tower(NumTowers) ->
	[ NumTowers | create_tower( NumTowers-1 ) ].

% Public, creates the towers with one tower having N disks.
create_towers(NumTowers) ->
	NTow = [lists:reverse(create_tower(NumTowers)), [], []],
	display_towers(NTow),
	NTow.

% Public, displays the towers array.
display_towers([Tower1, Tower2, Tower3]) ->
    io:format("-------------------------\n"),
    io:format("Tower 1: ~p\nTower 2: ~p\nTower 3: ~p\n", [lists:reverse(Tower1), lists:reverse(Tower2), lists:reverse(Tower3)]),
	io:format("-------------------------\n").

% Private, list helper
listssetnth(L, I, E) ->
	lists:sublist(L, I-1) ++ [E] ++ lists:nthtail(I, L).
	
% Public, moves a disk from one tower to another
move(Towers, From, To) when From > 0, To > 0, From =< 3, To =< 3 ->
	io:format("Moving a disk from Tower ~p to Tower ~p\n", [From, To]),

	Ap = hd(lists:nth(From, Towers)), % Find the disk being moved
	
	ToTower = lists:append([Ap], lists:nth(To, Towers)), % Tower that has been moved to
	FromTower = tl(lists:nth(From, Towers)), % Tower that was moved from (becomes its tl, without the head)
	
	%io:format("From Tower ~p: ~p\nTo Tower ~p: ~p\n", [From, FromTower, To, ToTower]),
	TowersPreResult = listssetnth(Towers, From, FromTower),
	TowersResult = listssetnth(TowersPreResult, To, ToTower),

	display_towers(TowersResult),
	TowersResult;
move(Towers, _, _) ->
	io:format("Range error. Make sure both From and To are > 0 and =< 3.\n"),
	display_towers(Towers),
	Towers.
	
% Private, moves n disks to a location legally, but requires a "spare" peg
move_n(Towers, 1, From, To, _) ->
	move(Towers, From, To); % Solves a tower size of 1: just move it to the destination.
move_n(Towers, N, From, To, Spare) ->
	Solved1 = move_n(Towers, N-1, From, Spare, To), % Move N-1 disks to the "spare" peg with this function
	Solved2 = move(Solved1, From, To), % Move the one disk remaining at our "From" peg to our "To" peg
	move_n(Solved2, N-1, Spare, To, From). % Move all disks on the "spare" peg onto said peg using "From" as our "spare" peg

% Public, counts number of disks on your towers
count_disks([Tower1, Tower2, Tower3]) ->
	length(Tower1) + length(Tower2) + length(Tower3).
	
% Public, solves hanoi problem from the start state
solve(Towers) ->
	N = count_disks(Towers),
	move_n(Towers, N, 1, 3, 2).
