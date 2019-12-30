% Program:  family.pl
% Source:   Prolog
%
% Purpose:  This is the sample program for the Prolog Lab in COMP9414/9814/3411.
%           It is a simple Prolog program to demonstrate how prolog works.
%           See lab.html for a full description.
%
% History:  Original code by Barry Drake



% parent(Parent, Child)
%
parent(albert, jim).
parent(albert, peter).
parent(jim, brian).
parent(john, darren).
parent(peter, lee).
parent(peter, sandra).
parent(peter, james).
parent(peter, kate).
parent(peter, kyle).
parent(brian, jenny).
parent(irene, jim).
parent(irene, peter).
parent(pat, brian).
parent(pat, darren).
parent(amanda, jenny).


% female(Person)
%
female(irene).
female(pat).
female(lee).
female(sandra).
female(jenny).
female(amanda).
female(kate).

% male(Person)
%
male(albert).
male(jim).
male(peter).
male(brian).
male(john).
male(darren).
male(james).
male(kyle).


% yearOfBirth(Person, Year).
%
yearOfBirth(irene, 1923).
yearOfBirth(pat, 1954).
yearOfBirth(lee, 1970).
yearOfBirth(sandra, 1973).
yearOfBirth(jenny, 2004).
yearOfBirth(amanda, 1979).
yearOfBirth(albert, 1926).
yearOfBirth(jim, 1949).
yearOfBirth(peter, 1945).
yearOfBirth(brian, 1974).
yearOfBirth(john, 1955).
yearOfBirth(darren, 1976).
yearOfBirth(james, 1969).
yearOfBirth(kate, 1975).
yearOfBirth(kyle, 1976).

oldest(Person, Result) :-
	male(Person),
	not(parent(_, Person)),
	Result = Person.

oldest(Person, Result) :-
	parent(Person1, Person),
	oldest(Person1, R),
	Result = R.

same_name(Person, Person).
same_name(Person1, Person2) :-
	parent(Person1, Person2),
	male(Person1).
same_name(Person1, Person2) :-
	parent(Person2, Person1),
	male(Person2).
same_name(Person1, Person2) :-
	oldest(Person1, G1),
	oldest(Person2, G2),
	G1 = G2.

% grandparent(Grandparent, Grandchildren) :- Grandparent is the grandparent of Grandchildren. 
%
grandparent(Grandparent, Grandchildren) :-
	parent(Grandparent, Parent),
	parent(Parent, Grandchildren).

% older(Person1, Person2) :- Person1 is older than Person2.
%
older(P1, P2) :-
	yearOfBirth(P1, Y1), 
	yearOfBirth(P2, Y2), 
	Y1 < Y2.

% siblings(Child1, Child2) :- Children1 and Children2 share the same parent
%
siblings(Child1, Child2) :-
	parent(X, Child1), 
	parent(X, Child2),
	Child1 \= Child2.

% olderBrother(Brother, Person) :- Brother and Person are siblings and Brother is older than Person and Brother is male.
%
olderBrother(Brother, Person) :-
	siblings(Brother, Person),
	older(Brother, Person),
	male(Brother).

%  descendant(Person, Descendant) :- Person is a descendant of Descendant.
%
descendant(Person, Descendant) :-
	parent(Person, Descendant).

descendant(Person, Descendant) :-
	parent(Person, X),
	descendant(X, Descendant).

% ancestor(A, B) :- B is the ancestor of A.
%
ancestor(A, B) :-
	parent(B, A).

ancestor(A, B) :-
	parent(B, X),
	ancestor(A, X).
% test(f(A,B,C), D) :- A = C and B = D.
%
test(f(A, B, C), D) :-
	A = C, B = D.

children(Parent, ChildList):-
	findall(D, parent(Parent,D), List),
	ChildList = List.

sibling(Person1, Person2):-
	parent(Person, Person1),
	parent(Person, Person2),
	Person1 \= Person2,
	male(Person).

sibling_list(Person, Siblings):-
	findall(D, sibling(D, Person), List),
	Siblings = List.
	
listCount([], Count):-
	Count = 0.
listCount(List, Count):-
	List = [_|Tail],
	listCount(Tail, Back),
	Count is 1 + Back.

countDescendants(Person, Count):-
	findall(D, descendant(Person, D), List),
	listCount(List, Count).

deepListCount(A, 1) :-
        A \= [], not(A = [_|_]).

deepListCount([], Count):-
    Count = 0.

deepListCount(List, Count):-
	List = [Head|Tail],
	Head \= [], not(Head = [_|_]),
	!,
	deepListCount(Tail, R),
	Count is R + 1.


deepListCount(List, Count):-
	List = [Head|Tail],
	Head = [_|_],
	!,
	deepListCount(Head, R1),
	deepListCount(Tail, R2),
	Count is R1 + R2.

