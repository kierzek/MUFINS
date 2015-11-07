# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="John"
__date__ ="$05-Mar-2014 13:22:46$"

def ArrayInsert(arr, objarr, i):
    return arr[:(i+1)] + objarr + arr[(i+1):]
