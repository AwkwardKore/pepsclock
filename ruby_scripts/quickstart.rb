require "serialport"
require 'google/apis/gmail_v1'
require 'googleauth'
require 'googleauth/stores/file_token_store'
require 'fileutils'

#Setup the serial port for communication between arduino and script
port_str = "/dev/cu.usbmodem1411"
baud_rate = 9600
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE
@sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

#Sends current time to the arduino
for i in 0..2
  hour = format('%02d', Time.now.hour)
  minute = format('%02d', Time.now.min)
  second = format('%02d', Time.now.sec)
  day = format('%02d', Time.now.day)
  month = format('%02d', Time.now.month)
  @sp.write("#{hour}:#{minute}:#{second}:#{day}:#{month}:#{Time.now.year}")
  sleep 1
end

#Setup the gmail api for the app
OOB_URI = 'urn:ietf:wg:oauth:2.0:oob'
APPLICATION_NAME = 'Gmail API Ruby Quickstart'
CLIENT_SECRETS_PATH = 'client_secret.json'
CREDENTIALS_PATH = File.join(Dir.home, '.credentials',
                             "gmail-ruby-quickstart.yaml")
SCOPE = Google::Apis::GmailV1::AUTH_GMAIL_MODIFY

##
# Ensure valid credentials, either by restoring from the saved credentials
# files or intitiating an OAuth2 authorization. If authorization is required,
# the user's default browser will be launched to approve the request.
#
# @return [Google::Auth::UserRefreshCredentials] OAuth2 credentials
def authorize
  FileUtils.mkdir_p(File.dirname(CREDENTIALS_PATH))

  client_id = Google::Auth::ClientId.from_file(CLIENT_SECRETS_PATH)
  token_store = Google::Auth::Stores::FileTokenStore.new(file: CREDENTIALS_PATH)
  authorizer = Google::Auth::UserAuthorizer.new(
    client_id, SCOPE, token_store)
  user_id = 'default'
  credentials = authorizer.get_credentials(user_id)
  if credentials.nil?
    url = authorizer.get_authorization_url(
      base_url: OOB_URI)
    puts "Open the following URL in the browser and enter the " +
         "resulting code after authorization"
    puts url
    code = gets
    credentials = authorizer.get_and_store_credentials_from_code(
      user_id: user_id, code: code, base_url: OOB_URI)
  end
  credentials
end

# Initialize the API
service = Google::Apis::GmailV1::GmailService.new
service.client_options.application_name = APPLICATION_NAME
service.authorization = authorize

#Checks if there is new email with keyword
user_id = 'me'
def check_for_alarm(user_id, service)
  while true
    # Show the user messages' id from certain email address
    result = service.list_user_messages(user_id, q: "from: dani.islas96@gmail.com is: unread", max_results: 1)
    if result.messages.nil?
      @sp.write("n")
      puts "No messages found on the inbox"
    else
      result.messages.each { |message| @id_last_message = message.id}

      #Get the subject of a message with certain id
      result = service.get_user_message(user_id, @id_last_message.to_s)
      puts "The message is: "
      message_subject =  "#{result.payload.headers[5].value}"
      puts "#{message_subject}"

      #Set the alarm if only numbers are found in the subject
      alarm_time = message_subject.gsub(/[^0-9]/, '')
      if alarm_time.length > 3
        alarm_time = alarm_time[0,4]
        alarm_hour = alarm_time[0,2].to_i
        alarm_minutes = alarm_time[2,4].to_i
        if alarm_hour.between?(0,23) and alarm_minutes.between?(0,60)
          @sp.write(0) if alarm_hour.between?(0,9)
          @sp.write("#{alarm_hour}")
          @sp.write(0) if alarm_minutes.between?(0,9)
          @sp.write("#{alarm_minutes}")
          puts "The alarm is set"
          ready = @sp.read(1)
          if ready == "r"
            service.trash_user_message(user_id, @id_last_message)
          end
        else
          @sp.write("n")
          puts "There's no alarm in the last message"
        end
      else
        @sp.write("n")
        puts "There's no alarm in the last message"
      end
    end
  end
end

check_for_alarm(user_id, service)
