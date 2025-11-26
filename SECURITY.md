# Security Policy

## Supported Versions

We actively support the following versions of SIMILAR with security updates:

| Version | Supported          |
| ------- | ------------------ |
| Latest  | :white_check_mark: |
| < 1.0   | :x:                |

## Reporting a Vulnerability

If you discover a security vulnerability in SIMILAR, please report it to us as follows:

### Contact

- **Email**: security@example.com (replace with actual security contact)
- **GitHub**: Create a private security advisory at https://github.com/gildasmorvan/similar/security/advisories

### What to Include

When reporting a security vulnerability, please include:

- A clear description of the vulnerability
- Steps to reproduce the issue
- Potential impact and severity
- Any suggested fixes or mitigations
- Your contact information for follow-up

### Response Timeline

- **Acknowledgment**: Within 48 hours
- **Initial Assessment**: Within 7 days
- **Fix Development**: Within 30 days for critical issues
- **Public Disclosure**: After fix is deployed and tested

## Security Considerations

### For Users

- Always use the latest stable version
- Validate input data in your simulations
- Be cautious with untrusted simulation models
- Use secure random seeds for reproducible results

### For Contributors

- Follow secure coding practices
- Validate all inputs and outputs
- Use safe memory management in C++ code
- Implement proper error handling
- Keep dependencies updated

## Known Security Considerations

- **Random Number Generation**: Uses standard library PRNGs - ensure proper seeding for security-sensitive applications
- **Network Interfaces**: Web server components should not be exposed to untrusted networks without proper authentication
- **File I/O**: Simulation data files should be validated before processing
- **Memory Usage**: Large simulations may consume significant memory - monitor resource usage

## Security Updates

Security updates will be:

- Released as patch versions
- Documented in release notes
- Announced through GitHub releases
- Tagged with appropriate severity levels
