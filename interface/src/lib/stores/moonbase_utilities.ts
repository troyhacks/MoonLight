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

export function getTimeAgo(timestamp: number, currentTime: number = Date.now()): string {
    const diff = currentTime - timestamp * 1000; // Convert seconds to milliseconds

    const seconds = Math.floor(diff / 1000);
    const minutes = Math.floor(seconds / 60);
    const hours = Math.floor(minutes / 60);
    const days = Math.floor(hours / 24);

    if (days > 0) return `${days} day${days > 1 ? 's' : ''} ago`;
    if (hours > 0) return `${hours} hour${hours > 1 ? 's' : ''} ago`;
    if (minutes > 0) return `${minutes} minute${minutes > 1 ? 's' : ''} ago`;
    return `${seconds} second${seconds > 1 ? 's' : ''} ago`;
}
