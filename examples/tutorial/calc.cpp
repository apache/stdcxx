/**************************************************************************
 *
 * calc.cpp - RPN Calculator -- Illustration of the use of stacks.
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <vector>
#include <stack>
#include <iostream>

#include <examples.h>    


// Simulate the behavior of a simple integer calculator.

struct Calculator
{
    int currentMemory () const {
        return data_.top ();
    }

    void pushOperand (int value) {
        data_.push (value);
    }

    void doOperator (char);

private:

    std::stack<int, std::vector<int, std::allocator<int> > > data_;
};


void Calculator::doOperator (char theOp)
{
    int tmp = data_.top ();
    data_.pop ();

    switch (theOp) {
    case '+': tmp = data_.top () + tmp; break;
    case '-': tmp = data_.top () - tmp; break;
    case '*': tmp = data_.top () * tmp; break;
    case '/': tmp = data_.top () / tmp; break;
    case '%': tmp = data_.top () % tmp; break;
    }

    data_.pop ();
    data_.push (tmp);
}


int main ()
{
    std::cout << "Calculator example program, from Chapter 8\n"
              << "Enter a legal RPN expression, end with p q (print and quit)"
              << std::endl;

    Calculator calc;

    for (char c; std::cin >> c; ) {

        int intval;

        switch (c) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            std::cin.putback (c);
            std::cin >> intval;
            calc.pushOperand (intval);
            break;
        case '+': case '-': case '*': case '/': case '%':
            calc.doOperator (c);
            break;
        case 'p':
            std::cout << calc.currentMemory () << std::endl;
            break;
        case 'q':
            std::cout << "End calculator program" << std::endl;
            return 0;   // quit program
        }
    }

    return 0;
}
