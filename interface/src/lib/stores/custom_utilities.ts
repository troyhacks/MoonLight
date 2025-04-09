function isLowerCase(s: string) {
    return s.toLowerCase() == s
}

export function initCap(s: string) {
    if (typeof s !== 'string') return '';
    let result = "";
    for (let i = 0; i < s.length; i++) {
        if (i==0) //first uppercase
        result += s.charAt(i).toUpperCase();
        else if (!isLowerCase(s.charAt(i)) && isLowerCase(s.charAt(i-1))) //uppercase (previous not uppercase) => add space
        // else if (!isLowerCase(s.charAt(i)) && isLowerCase(s.charAt(i-1)) && (i+1 >= s.length || isLowerCase(s.charAt(i+1))))
        result += " " + s.charAt(i);
        // else if (!isLowerCase(s.charAt(i)) && !isLowerCase(s.charAt(i-1)) && (i+1 >= s.length || isLowerCase(s.charAt(i+1))))
        //   result += " " + s.charAt(i);
        else if (s.charAt(i) == '-' || s.charAt(i) == '_') //- and _ is space
        result += " ";
        else
        result+=s.charAt(i);
    }
    return result;
}