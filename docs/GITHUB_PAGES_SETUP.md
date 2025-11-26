# GitHub Pages Setup

This document explains how to set up GitHub Pages for the SIMILAR documentation.

## Prerequisites

1. **Repository Settings Access**: You need admin access to the repository
2. **GitHub Pages Enabled**: Pages must be enabled in repository settings

## Setup Instructions

### 1. Enable GitHub Pages

1. Go to your repository on GitHub
2. Click on **Settings** tab
3. Scroll down to **Pages** section in the left sidebar
4. Under **Source**, select **GitHub Actions**

### 2. Initial Deployment

The documentation will be automatically deployed when:
- You push changes to `main`/`master` branch affecting documentation files
- The CI workflow completes successfully

### 3. Custom Domain (Optional)

If you want to use a custom domain (like `similar.readthedocs.io`):

1. Go to repository **Settings** → **Pages**
2. In the **Custom domain** field, enter your domain
3. Add a CNAME record pointing to `gildasmorvan.github.io` (or your GitHub Pages domain)

### 4. Manual Deployment

You can also trigger documentation deployment manually:

1. Go to **Actions** tab in your repository
2. Find the **Documentation** workflow
3. Click **Run workflow** button

## Workflow Details

The documentation deployment uses:
- **Build**: MkDocs with Material theme
- **Deployment**: GitHub Actions Pages deployment
- **Trigger**: Changes to docs, README, or markdown files
- **URL**: `https://gildasmorvan.github.io/similar/` (or your custom domain)

## Troubleshooting

### Permission Denied Error
If you see permission errors during deployment:
1. Ensure GitHub Pages is enabled in repository settings
2. Check that the source is set to "GitHub Actions"
3. Verify the workflow has the correct permissions

### Build Failures
- Check the Actions tab for detailed error logs
- Verify MkDocs configuration is valid
- Ensure all required files are present

### Domain Issues
- Wait 24-48 hours for DNS changes to propagate
- Check that your CNAME record is correctly configured
- Verify the domain ownership if using custom domain

## File Structure

```
docs/
├── index.md                 # Home page
├── GITHUB_PAGES_SETUP.md    # This file
└── ...                      # Other documentation
```

## Support

For issues with GitHub Pages setup, refer to:
- [GitHub Pages Documentation](https://docs.github.com/en/pages)
- [MkDocs Documentation](https://www.mkdocs.org/)
