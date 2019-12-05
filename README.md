
# Apris

## Information
 
Apris is a string processor.

It allows insertion of sub-strings within a string using control codes. Sets (banks) of sub-strings can stored and the string can be processed whichever bank is required at the time.

Control codes are integer values but a string can also be mapped to those integers to allow more readable strings.

Control sub-strings can also be capitalised automatically using coded values.

It also allows alternatives directly contained within the string itself. Which alternative to use can be set per bank or on a global level (and changed easily at any time as required).

## Examples

### A very basic example

`"You hit your enemy with your $1$."`
If there are three banks, each with just one string, this could have three different outputs.  
If bank 1 had `"axe"`, bank 2 had `"sword"`, bank 3 had `"fist"`, the results would be:  
`"You hit your enemy with your axe."`  
`"You hit your enemy with your sword."`  
`"You hit your enemy with your fist."`

### A basic but more clear example

Assume exactly the same banks as before in the very basic example but this time we have set up a string mapped to the control: `"weapon"` is mapped to 1.  
Instead, the string we can use would be:  
`"You hit your enemy with your $weapon$." ` 
and the three results would again be:  
`"You hit your enemy with your axe."`  
`"You hit your enemy with your sword."`  
`"You hit your enemy with your fist."`

### A useful example

One common time when replacement of certain parts of a string (words in this case) is required, especially in story text, is when gender is referenced.  
Using a bank per gender, we can simply choose which bank to use for the gender. That way, we can write only one string that covers all.  
For a 'cover most' example, we can use 3 banks: male, female and neutral. Obviously more can added if necessary.  
We need 4 words per bank to cover common pronouns:  
`"he"`, `"him"`, `"his"`, `"his"`  
`"she"`, `"her"`, `"her"`, `"hers"`  
`"they"`, `"them"`, `"their"`, `"theirs"`  
We can then just the control values to write a sentence:  
`"Any time $1$ want^s^, $1$ can get $3$ thing back to $2$ from me."`  
Gives:  
MALE: `"Any time he wants, he can get his thing back to him from me."`  
FEMALE: `"Any time she wants, she can get her thing back to her from me."`  
NEUTRAL: `"Any time they want, they can get their thing back to them from me."`  
It's also possible to use control mapping to allow you to write the based on one gender and get the same results:  
`"Any time $he$ want^s^, $he$ can get $his$ thing back to $him$ from me.\n"`  
You can, of course, which gender you prefer to use as the basis; you can set up the control maps however you like.

### A more thorough example

Sticking with the gender banks above, we can add in capitalisation processing that allows us to force one of the words to be capitalised using any locale.  
When using values for the codes, you can simply make that value its negative equivalent to make it capitalise that string.  
When using control-mapped strings, you can prefix (inside the control limiters) that string with the capital control character. This removes any capitalisation of the sub-string before processing and then forces capitalisation when adding the final string.

Here's a more full (but still quite nonsensical) example:  
`"$_He$ told me that $he$ want^s^ the thing if you would allow $him$ to have it."`  
`"Any time $he$ want^s^, $he$ can get $his$ thing back to $him$ from me."`  
`"$_He$ ^is|are^ very happy because it is $his_$. $_He$ should get it back $him$self."`  
Gives:  
**MALE**
`"He told me that he wants the thing if you would allow him to have it."`  
`"Any time he wants, he can get his thing back to him from me."`  
`"He is very happy because it is his. He should get it back himself."`  
**FEMALE**
`"She told me that she wants the thing if you would allow her to have it."`  
`"Any time she wants, she can get her thing back to her from me."`  
`"She is very happy because it is hers. She should get it back herself."`  
**NEUTRAL**
`"They told me that they want the thing if you would allow them to have it."`  
`"Any time they want, they can get their thing back to them from me."`  
`"They are very happy because it is theirs. They should get it back themself."`  
Note that because there are two versions of "his", for the second control mapped version, we've added an underline to distinguish the difference.  
The underline at the beginning, however, is the capitalisation character.
