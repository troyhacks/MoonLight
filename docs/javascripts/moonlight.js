// MoonLight Homepage JavaScript Enhancement
document.addEventListener('DOMContentLoaded', function() {
    // Scroll Progress Bar
    createScrollProgress();
    
    // Intersection Observer for Animations
    setupScrollAnimations();
    
    // Animated Counters
    setupCounterAnimations();
    
    // Floating Elements
    createFloatingElements();
    
    // Enhanced Interactions
    setupInteractiveElements();
    
    // Parallax Effect
    setupParallaxEffect();
});

// Create Scroll Progress Bar
function createScrollProgress() {
    const progressBar = document.createElement('div');
    progressBar.className = 'scroll-progress';
    document.body.appendChild(progressBar);
    
    window.addEventListener('scroll', () => {
        const scrollPercent = (window.scrollY / (document.documentElement.scrollHeight - window.innerHeight)) * 100;
        progressBar.style.width = scrollPercent + '%';
    });
}

// Setup Scroll Animations
function setupScrollAnimations() {
    const observerOptions = {
        threshold: 0.1,
        rootMargin: '0px 0px -50px 0px'
    };
    
    const observer = new IntersectionObserver((entries) => {
        entries.forEach((entry, index) => {
            if (entry.isIntersecting) {
                // Add staggered delay for multiple elements
                setTimeout(() => {
                    entry.target.classList.add('animate');
                }, index * 100);
            }
        });
    }, observerOptions);
    
    // Observe elements for animation
    const animateElements = document.querySelectorAll(
        '.moonlight-stats, .features-section, .performance-section, ' +
        '.feature-card, .moonlight-stat, .moonlight-buttons, ' +
        '.slide-in-left, .slide-in-right, .scale-in'
    );
    
    animateElements.forEach(el => observer.observe(el));
}

// Animated Counters for Stats
function setupCounterAnimations() {
    const stats = document.querySelectorAll('.moonlight-stat-number');
    const statsObserver = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                animateCounter(entry.target);
                statsObserver.unobserve(entry.target);
            }
        });
    }, { threshold: 0.5 });
    
    stats.forEach(stat => statsObserver.observe(stat));
}

function animateCounter(element) {
    const text = element.textContent;
    const number = parseInt(text.replace(/\D/g, ''));
    const suffix = text.replace(/[\d,]/g, '');
    
    if (isNaN(number)) return;
    
    const duration = 2000;
    const steps = 60;
    const increment = number / steps;
    const stepTime = duration / steps;
    
    let current = 0;
    const timer = setInterval(() => {
        current += increment;
        if (current >= number) {
            current = number;
            clearInterval(timer);
        }
        
        if (number >= 1000) {
            element.textContent = Math.floor(current).toLocaleString() + suffix;
        } else {
            element.textContent = Math.floor(current) + suffix;
        }
    }, stepTime);
}

// Create Floating Elements
function createFloatingElements() {
    const hero = document.querySelector('.moonlight-hero');
    if (!hero) return;
    
    for (let i = 0; i < 3; i++) {
        const floating = document.createElement('div');
        floating.className = 'floating-element';
        floating.innerHTML = ['🌙', '⭐', '✨'][i];
        floating.style.cssText = `
            top: ${Math.random() * 70 + 10}%;
            left: ${Math.random() * 80 + 10}%;
            font-size: ${Math.random() * 20 + 20}px;
            animation-duration: ${Math.random() * 4 + 4}s;
        `;
        hero.appendChild(floating);
    }
}

// Interactive Elements
function setupInteractiveElements() {
    // Feature cards tilt effect
    const cards = document.querySelectorAll('.feature-card, .moonlight-stat');
    cards.forEach(card => {
        card.addEventListener('mousemove', handleCardTilt);
        card.addEventListener('mouseleave', resetCardTilt);
    });
    
    // Button ripple effect
    const buttons = document.querySelectorAll('.moonlight-btn');
    buttons.forEach(button => {
        button.addEventListener('click', createRipple);
    });
    
    // Stats card click effect
    const statCards = document.querySelectorAll('.moonlight-stat');
    statCards.forEach(card => {
        card.addEventListener('click', function() {
            this.style.transform = 'scale(0.95)';
            setTimeout(() => {
                this.style.transform = '';
            }, 150);
        });
    });
}

function handleCardTilt(e) {
    const card = e.currentTarget;
    const rect = card.getBoundingClientRect();
    const centerX = rect.left + rect.width / 2;
    const centerY = rect.top + rect.height / 2;
    const deltaX = (e.clientX - centerX) / (rect.width / 2);
    const deltaY = (e.clientY - centerY) / (rect.height / 2);
    
    card.style.transform = `
        perspective(1000px) 
        rotateX(${deltaY * -5}deg) 
        rotateY(${deltaX * 5}deg) 
        translateY(-10px)
    `;
}

function resetCardTilt(e) {
    e.currentTarget.style.transform = '';
}

function createRipple(e) {
    const button = e.currentTarget;
    const ripple = document.createElement('span');
    const rect = button.getBoundingClientRect();
    const size = Math.max(rect.width, rect.height);
    const x = e.clientX - rect.left - size / 2;
    const y = e.clientY - rect.top - size / 2;
    
    ripple.style.cssText = `
        position: absolute;
        width: ${size}px;
        height: ${size}px;
        left: ${x}px;
        top: ${y}px;
        background: rgba(255, 255, 255, 0.3);
        border-radius: 50%;
        transform: scale(0);
        animation: ripple 0.6s linear;
        pointer-events: none;
    `;
    
    button.style.position = 'relative';
    button.style.overflow = 'hidden';
    button.appendChild(ripple);
    
    ripple.addEventListener('animationend', () => ripple.remove());
}

// Parallax Effect
function setupParallaxEffect() {
    window.addEventListener('scroll', () => {
        const scrolled = window.pageYOffset;
        const hero = document.querySelector('.moonlight-hero');
        const performance = document.querySelector('.performance-section');
        
        if (hero) {
            hero.style.transform = `translateY(${scrolled * 0.5}px)`;
        }
        
        if (performance) {
            performance.style.transform = `translateY(${scrolled * 0.2}px)`;
        }
    });
}

// Add ripple animation CSS
const rippleStyle = document.createElement('style');
rippleStyle.textContent = `
    @keyframes ripple {
        to {
            transform: scale(4);
            opacity: 0;
        }
    }
`;
document.head.appendChild(rippleStyle);

// Dynamic theme color changes based on scroll
window.addEventListener('scroll', () => {
    const scrollPercent = window.scrollY / (document.documentElement.scrollHeight - window.innerHeight);
    const hue = 250 + (scrollPercent * 30); // From purple to pink
    document.documentElement.style.setProperty('--moonlight-primary', `hsl(${hue}, 60%, 70%)`);
});

// Add some sparkle effects on click
document.addEventListener('click', (e) => {
    if (Math.random() > 0.7) {
        createSparkle(e.clientX, e.clientY);
    }
});

function createSparkle(x, y) {
    const sparkle = document.createElement('div');
    sparkle.innerHTML = '✨';
    sparkle.style.cssText = `
        position: fixed;
        left: ${x}px;
        top: ${y}px;
        font-size: 16px;
        pointer-events: none;
        z-index: 9999;
        animation: sparkleAnim 1s ease-out forwards;
    `;
    
    document.body.appendChild(sparkle);
    
    setTimeout(() => sparkle.remove(), 1000);
}

const sparkleStyleAnim = document.createElement('style');
sparkleStyleAnim.textContent = `
    @keyframes sparkleAnim {
        0% {
            opacity: 1;
            transform: scale(0) rotate(0deg);
        }
        100% {
            opacity: 0;
            transform: scale(1.5) rotate(180deg) translateY(-50px);
        }
    }
`;
document.head.appendChild(sparkleStyleAnim);