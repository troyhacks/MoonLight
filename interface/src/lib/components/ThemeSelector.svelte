<script lang="ts">
  import { onMount } from 'svelte';
  import { telemetry } from '$lib/stores/telemetry';
  
  let isOpen = $state(false);
  let mediaQuery = $state<MediaQueryList | null>(null);
  let currentTheme = $state('auto');
  
  const themes = [
    { value: 'light', label: 'Light', icon: '☀️' },
    { value: 'dark', label: 'Dark', icon: '🌙' },
    { value: 'auto', label: 'Auto', icon: '🌗' }
  ];
  
  // Initialize theme from localStorage or default to 'auto'
  onMount(() => {
    currentTheme = localStorage.getItem('theme-preference') || 'auto';
  });
  
  // Function to apply theme to HTML element
  function applyTheme(selectedTheme: string) {
    const html = document.documentElement;
    
    if (selectedTheme === 'auto') {
      // Remove explicit theme, let CSS handle it
      html.removeAttribute('data-theme');
    } else if (selectedTheme === 'light') {
      html.setAttribute('data-theme', 'corporate');
    } else if (selectedTheme === 'dark') {
      html.setAttribute('data-theme', 'business');
    }
  }
  
  // Handle theme change
  function handleThemeChange(newTheme: string) {
    localStorage.setItem('theme-preference', newTheme);
    
    applyTheme(newTheme);
    isOpen = false;
  }
  
  // Listen for system theme changes when in auto mode
  function handleMediaQueryChange(e: MediaQueryListEvent) {
    if (currentTheme === 'auto') {
      applyTheme('auto');
    }
  }
  
  // Watch for theme changes
  $effect(() => {
    applyTheme(currentTheme);
  });
  
  onMount(() => {
    // Set up media query listener for system theme changes
    mediaQuery = window.matchMedia('(prefers-color-scheme: dark)');
    mediaQuery.addEventListener('change', handleMediaQueryChange);
    
    // Apply initial theme
    applyTheme(currentTheme);
    
    // Cleanup
    return () => {
      if (mediaQuery) {
        mediaQuery.removeEventListener('change', handleMediaQueryChange);
      }
    };
  });
  
  // Close dropdown when clicking outside
  function handleClickOutside(event: Event) {
    const target = event.target as Element | null;
    if (!target?.closest('.theme-selector')) {
      isOpen = false;
    }
  }
  
  onMount(() => {
    document.addEventListener('click', handleClickOutside);
    return () => document.removeEventListener('click', handleClickOutside);
  });
  
  // Get current theme icon
  function getCurrentThemeIcon() {
    return themes.find(t => t.value === currentTheme)?.icon || '🌗';
  }
</script>

<div class="theme-selector relative flex-none">
  <button 
    class="btn btn-square btn-ghost h-9 w-10" 
    onclick={() => isOpen = !isOpen}
    aria-label="Select theme"
    title="Theme: {themes.find(t => t.value === currentTheme)?.label || 'Auto'}"
  >
    <span class="text-lg">
      {getCurrentThemeIcon()}
    </span>
  </button>

  {#if isOpen}
    <div class="absolute right-0 mt-2 w-32 bg-base-100 rounded-lg shadow-lg border border-base-300 z-50">
      <ul class="py-1">
        {#each themes as themeOption}
          <li>
            <button
              class="w-full px-3 py-2 text-left hover:bg-base-200 flex items-center gap-2 transition-colors text-sm {currentTheme === themeOption.value ? 'bg-base-200 font-semibold' : ''}"
              onclick={() => handleThemeChange(themeOption.value)}
            >
              <span class="text-base">{themeOption.icon}</span>
              <span>{themeOption.label}</span>
              {#if currentTheme === themeOption.value}
                <svg class="w-3 h-3 ml-auto text-primary" fill="currentColor" viewBox="0 0 20 20">
                  <path fill-rule="evenodd" d="M16.707 5.293a1 1 0 010 1.414l-8 8a1 1 0 01-1.414 0l-4-4a1 1 0 011.414-1.414L8 12.586l7.293-7.293a1 1 0 011.414 0z" clip-rule="evenodd" />
                </svg>
              {/if}
            </button>
          </li>
        {/each}
      </ul>
    </div>
  {/if}
</div>

<style>
  .theme-selector {
    z-index: 1000;
  }
  
  .theme-selector div {
    animation: dropdownOpen 0.15s ease-out;
  }
  
  @keyframes dropdownOpen {
    from {
      opacity: 0;
      transform: translateY(-5px) scale(0.95);
    }
    to {
      opacity: 1;
      transform: translateY(0) scale(1);
    }
  }
</style>