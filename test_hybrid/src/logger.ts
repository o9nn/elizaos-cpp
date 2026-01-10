// ElizaOS Logger Implementation
export class Logger {
    private prefix: string;

    constructor(prefix: string = 'ElizaOS') {
        this.prefix = prefix;
    }

    log(level: string, message: string): void {
        const timestamp = new Date().toISOString();
        console.log(`[${timestamp}] [${this.prefix}] [${level}] ${message}`);
    }

    info(message: string): void {
        this.log('INFO', message);
    }

    warn(message: string): void {
        this.log('WARN', message);
    }

    error(message: string): void {
        this.log('ERROR', message);
    }

    debug(message: string): void {
        this.log('DEBUG', message);
    }
}

export function createLogger(prefix?: string): Logger {
    return new Logger(prefix);
}
