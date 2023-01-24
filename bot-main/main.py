import asyncio
import time
import uuid

# IMPORT DISCORD.PY. ALLOWS ACCESS TO DISCORD'S API.
import discord

# IMPORT THE OS MODULE.
import os
import requests
import wget
# IMPORT LOAD_DOTENV FUNCTION FROM DOTENV MODULE.
from dotenv import load_dotenv
from discord.ext import commands

import sys # Useful for subprocesses

# LOADS THE .ENV FILE THAT RESIDES ON THE SAME LEVEL AS THE SCRIPT.
load_dotenv()
input_queue = asyncio.Queue()
output_queue = asyncio.Queue()
trash_queue = asyncio.Queue()
# GRAB THE API TOKEN FROM THE .ENV FILE.
DISCORD_TOKEN = os.getenv("DISCORD_TOKEN")

# GETS THE CLIENT OBJECT FROM DISCORD.PY. CLIENT IS SYNONYMOUS WITH BOT.
bot = discord.Client()
bot = commands.Bot(command_prefix='!')

async def process_mp3(input_file):
    started_at = time.monotonic()

    in_wav_name = generate_wav_name()
    out_wav_name = generate_wav_name()
    out_mp3_name = generate_mp3_name()

    await ffmpeg_mp3_to_wav(input_file, in_wav_name)
    await fxbassboost(in_wav_name, out_wav_name)
    await ffmpeg_wav_to_mp3(out_wav_name, out_mp3_name)
    
    process_time = time.monotonic() - started_at
    print(f"time to process: {process_time}")

    # Delete all the input files.
    await trash_queue.put(in_wav_name)
    await trash_queue.put(out_wav_name)
    await trash_queue.put(input_file)

    # Return output_file
    return out_mp3_name
    
async def process_mp4(input_file, fry_it = False):
    started_at = time.monotonic()

    in_wav_name = generate_wav_name()
    out_wav_name = generate_wav_name()
    secondary_mp4_name = input_file
    if fry_it:
        secondary_mp4_name = generate_mp4_name()
    out_mp4_name = generate_mp4_name()

    await ffmpeg_mp4_to_wav(input_file, in_wav_name)
    await fxbassboost(in_wav_name, out_wav_name)
    if fry_it:
        await fxvideo(input_file, secondary_mp4_name)
    await ffmpeg_wav_to_mp4(out_wav_name, secondary_mp4_name, out_mp4_name)
    
    process_time = time.monotonic() - started_at
    print(f"time to process: {process_time}")

    # Delete all the input files.
    await trash_queue.put(in_wav_name)
    await trash_queue.put(out_wav_name)
    if fry_it:
        await trash_queue.put(secondary_mp4_name)
    await trash_queue.put(input_file)

    # Return output_file
    return out_mp4_name
    
async def process_jpg(input_file):
    started_at = time.monotonic()
    
    out_jpg_name = generate_jpg_name()
    
    await image_distortion(input_file, out_jpg_name)
    
    process_time = time.monotonic() - started_at
    print(f"time to process: {process_time}")
    
    await trash_queue.put(input_file)
    
    return out_jpg_name

async def process_png(input_file):
    started_at = time.monotonic()
    
    out_png_name = generate_png_name()
    
    await image_distortion(input_file, out_png_name)
    
    process_time = time.monotonic() - started_at
    print(f"time to process: {process_time}")
    
    await trash_queue.put(input_file)
    
    return out_png_name

async def process_wav(input_file):
    started_at = time.monotonic()

    out_wav_name = generate_wav_name()
    out_mp3_name = generate_mp3_name()
    in_wav_name = generate_wav_name()
    
    await ffmpeg_wav_to_wav(input_file, in_wav_name)
    await fxbassboost(in_wav_name, out_wav_name)
    await ffmpeg_wav_to_mp3(out_wav_name, out_mp3_name)
    
    process_time = time.monotonic() - started_at
    print(f"time to process: {process_time}")

    # Delete all the input files.
    await trash_queue.put(input_file)
    await trash_queue.put(out_wav_name)
    await trash_queue.put(in_wav_name)
    
    # Return output_file
    return out_mp3_name
    
async def process_gif(input_file):
    in_mp4_name = generate_mp4_name()
    out_gif_name = generate_gif_name()
    await ffmpeg_gif_to_mp4(input_file, in_mp4_name)
    await fxvideo(input_file, in_mp4_name)

    await ffmpeg_mp4_to_gif(in_mp4_name, out_gif_name)

    await trash_queue.put(in_mp4_name)
    await trash_queue.put(input_file)
    # Return output_file
    return out_gif_name

async def handle_mp3_message(message, message2):
    # Save input file
    input_filename = generate_mp3_name()
    await message.attachments[0].save(input_filename)
    
    # Send back output file
    output_filename = await process_mp3(input_filename)
    await message.channel.send(file=discord.File(output_filename))
    await message.channel.send("Here's that distorted mp3 you ordered, "+message2.author.mention+" >:)")
    
    # Don't need the otuput file anymore.
    await trash_queue.put(output_filename)
    
async def handle_mp4_message(message, message2, fry_it = False):

    # Save input file
    input_filename = generate_mp4_name()
    await message.attachments[0].save(input_filename)
    
    # Send back output file
    output_filename = await process_mp4(input_filename, fry_it)
    await message.channel.send(file=discord.File(output_filename))
    await message.channel.send("Here's that distorted mp4 you ordered "+message2.author.mention+" >:)")
    
    # Don't need the otuput file anymore.
    await trash_queue.put(output_filename)

async def handle_jpg_message(message, message2):
    input_filename = generate_jpg_name()
    await message.attachments[0].save(input_filename)
    
    # Send back output file
    output_filename = await process_jpg(input_filename)
    await message.channel.send(file=discord.File(output_filename))
    await message.channel.send("Here's that distorted jpg you ordered "+message2.author.mention+" >:)")
    
    # Don't need the otuput file anymore.
    await trash_queue.put(output_filename)

async def handle_png_message(message, message2):
    input_filename = generate_png_name()
    await message.attachments[0].save(input_filename)
    
    # Send back output file
    output_filename = await process_png(input_filename)
    await message.channel.send(file=discord.File(output_filename))
    await message.channel.send("Here's that distorted png you ordered "+message2.author.mention+" >:)")
    
    # Don't need the otuput file anymore.
    await trash_queue.put(output_filename)
    
async def handle_wav_message(message, message2):
    # Save input file
    input_filename = generate_wav_name()
    await message.attachments[0].save(input_filename)
    
    # Send back output file
    output_filename = await process_wav(input_filename)
    await message.channel.send(file=discord.File(output_filename))
    await message.channel.send("Here's that distorted wav you ordered "+message2.author.mention+" >:)")
    
    # Don't need the otuput file anymore.
    await trash_queue.put(output_filename)
    
async def handle_gif_message(message, message2, fry_it=True):
    # Save input file
    input_filename = generate_gif_name()
    await message.attachments[0].save(input_filename)

    # Send back output file
    output_filename = await process_gif(input_filename)
    await message.channel.send(file=discord.File(output_filename))
    await message.channel.send("Here's that distorted gif you ordered "+message2.author.mention+" >:)")

    # Don't need the otuput file anymore.
    await trash_queue.put(output_filename)
    
async def handle_gif2_message(message, message2, fry_it=True):
    # Save input file
    input_filename = generate_gif_name()
    response = requests.get(message)
    open(input_filename, "wb").write(response.content)
    #await message.attachments[0].save(input_filename)

    # Send back output file
    output_filename = await process_gif(input_filename)
    await message2.channel.send(file=discord.File(output_filename))
    await message2.channel.send("Here's that distorted gif you ordered "+message2.author.mention+" >:)")

    # Don't need the otuput file anymore.
    await trash_queue.put(output_filename)

@bot.command()
async def upload_file(ctx):
    if(len(ctx.message.attachments) == 1):
        print(f"Received message with attachment: {ctx.message.attachments[0].url}")
        if("gif" in ctx.message.content.lower()):
            await handle_gif2_message(message.content.lower(), message, True)
            await bot.prcess_commands(message)
        elif (ctx.message.attachments[0].url.lower().endswith('mp3')):
            await handle_mp3_message(ctx.message)
            return
        elif (ctx.message.attachments[0].url.lower().endswith('mp4')):
            fry_it = False
            #print(ctx.message.content.lower())
            text = ctx.message.content.lower()
            if "fry" in text and "it" in text:
                fry_it = True
            await handle_mp4_message(ctx.message, fry_it)
            return
        elif (ctx.message.attachments[0].url.lower().endswith('jpg')):
            await handle_jpg_message(ctx.message)
            return
        elif (ctx.message.attachments[0].url.lower().endswith('png')):
            await handle_png_message(ctx.message)
            return
        elif (ctx.message.attachments[0].url.lower().endswith('wav')):
            await handle_wav_message(ctx.message)
            return
        elif (ctx.message.attachments[0].url.lower().endswith('gif')):
            await handle_gif_message(ctx.message, True)
            return
    # If we didn't return, no valid file type was entered
    await ctx.message.channel.send("Please ensure your message has a single .mp3/.mp4/.jpg/.png/.wav/.gif attachment")        
@bot.event
async def on_message(message):
    if (message.author == bot.user):
        await bot.process_commands(message)
    elif isinstance(message.channel, discord.DMChannel):
        if("gif" in message.content.lower()):
            await handle_gif2_message(message.content.lower(), message, True)
            await bot.process_commands(message)
        elif(len(message.attachments) >= 1) and (message.attachments[0].url.lower().endswith('mp3')):
            await handle_mp3_message(message)
            await bot.process_commands(message)
        elif(len(message.attachments) >= 1) and (message.attachments[0].url.lower().endswith('mp4')):
            fry_it = False
            text = message.content.lower()
            if "fry" in text and "it" in text:
                fry_it = True
            await handle_mp4_message(message, fry_it)
            await bot.process_commands(message)
        elif (len(message.attachments) >= 1) and ("gif" in message.attachments[0].url.lower()):
            await handle_gif_message(message, True)
            await bot.process_commands(message)
            return
        elif(len(message.attachments) >= 1) and (message.attachments[0].url.lower().endswith('jpg')):
            await handle_jpg_message(message)
            await bot.process_commands(message)
        elif(len(message.attachments) >= 1) and (message.attachments[0].url.lower().endswith('png')):
            await handle_png_message(message)
            await bot.process_commands(message)
        elif(len(message.attachments) >= 1) and (message.attachments[0].url.lower().endswith('png')):
            await handle_png_message(message)
            await bot.process_commands(message)
        elif(len(message.attachments) >= 1) and (message.attachments[0].url.lower().endswith('wav')):
            await handle_wav_message(message)
            await bot.process_commands(message)
        else:
            await message.author.send("Please ensure your message has a single .mp3/.mp4/.jpg/.png/.wav/.gif attachment")
            await bot.process_commands(message)
    elif bot.user.mentioned_in(message):
        if message.mention_everyone:
            return
        next = discord.utils.get(message.guild.text_channels, name=message.channel.name)
        messages = await next.history(limit=6).flatten()
        for i in range(len(messages)):
            prev = messages[i]
            if (prev.author == bot.user):
                hello = 0
            elif("gif" in prev.content.lower()):
                await handle_gif2_message(prev.content.lower(), message, True)
                await bot.process_commands(message)
                return
            elif(len(prev.attachments) >= 1) and (prev.attachments[0].url.lower().endswith('mp3')):
                await handle_mp3_message(prev, message)
                await bot.process_commands(message)
                return
            elif(len(prev.attachments) >= 1) and (prev.attachments[0].url.lower().endswith('mp4')):
                fry_it = False
                text = message.content.lower() # Get the frying parameter from the current message, not from the video source mssage
                if "fry" in text and "it" in text:
                    fry_it = True
            
                await handle_mp4_message(prev, message, fry_it)
                await bot.process_commands(message)
                return
            elif (len(prev.attachments) >= 1) and (prev.attachments[0].url.lower().endswith('gif')):
                fry_it = True
                await handle_gif_message(prev, message, fry_it)
                await bot.process_commands(message)
                return
            elif(len(prev.attachments) >= 1) and (prev.attachments[0].url.lower().endswith('jpg')):
                await handle_jpg_message(prev, message)
                await bot.process_commands(message)
                return
            elif(len(prev.attachments) >= 1) and (prev.attachments[0].url.lower().endswith('png')):
                await handle_png_message(prev, message)
                await bot.process_commands(message)
                return
            elif(len(prev.attachments) >= 1) and (prev.attachments[0].url.lower().endswith('wav')):
                await handle_wav_message(prev, message)
                await bot.process_commands(message)
                return
            #else:
                #await message.channel.send(prev.content)
                #await message.channel.send(len(prev.attachments))
                #await bot.process_commands(message)
                #break
        await message.channel.send("No user .mp3/.mp4/.img/.png/.wav/.gif file was found in the previous 5 messages")
    await bot.process_commands(message)
  

def generate_jpg_name():
    return uuid.uuid4().hex + ".jpg"
    
def generate_png_name():
    return uuid.uuid4().hex + ".png"
    
def generate_wav_name():
    return uuid.uuid4().hex + ".wav"
    
def generate_mp3_name():
    return uuid.uuid4().hex + ".mp3"
    
def generate_mp4_name():
    return uuid.uuid4().hex + ".mp4"
    
def generate_gif_name():
    return uuid.uuid4().hex + ".gif"

async def image_distortion(image_in, image_out):
    proc = await asyncio.create_subprocess_exec("vidfx/imgfx", image_in, image_out)
    
    await proc.wait()

async def ffmpeg_gif_to_mp4(file_in, file_out):
    proc = await asyncio.create_subprocess_exec("ffmpeg", "-f", "gif", "-i", file_in, file_out,
        stdout=asyncio.subprocess.DEVNULL,
        stderr=asyncio.subprocess.DEVNULL)
    await proc.wait()
    
async def ffmpeg_mp4_to_gif(file_in, file_out):
    proc = await asyncio.create_subprocess_exec("ffmpeg", "-i", file_in, file_out,
        stdout=asyncio.subprocess.DEVNULL,
        stderr=asyncio.subprocess.DEVNULL)
    await proc.wait()

async def ffmpeg_mp4_to_wav(file_in, file_out):
    proc = await asyncio.create_subprocess_exec("ffmpeg", "-y", "-i", file_in, file_out,
        stdout=asyncio.subprocess.DEVNULL,
        stderr=asyncio.subprocess.DEVNULL)
        
    await proc.wait()
    
async def ffmpeg_wav_to_mp4(wav_in, mp4_in, mp4_out):
    # This code is based off of:
    # https://ochremusic.com/2016/07/05/replacing-video-audio-using-ffmpeg/
    proc = await asyncio.create_subprocess_exec("ffmpeg", "-y",
        "-i", mp4_in,
        "-i", wav_in,
        "-map", "0:0",
        "-map", "1:0",
        "-c:v", "copy",
        "-c:a", "aac",
        "-b:a", "256k",
        "-shortest",
        mp4_out,
        
        stdout=asyncio.subprocess.DEVNULL,
        stderr=asyncio.subprocess.DEVNULL)
        
    await proc.wait()

async def ffmpeg_mp3_to_wav(file_in, file_out):
    proc = await asyncio.create_subprocess_exec("ffmpeg", "-y", "-i", file_in, file_out,
        stdout=asyncio.subprocess.DEVNULL,
        stderr=asyncio.subprocess.DEVNULL)
    
    await proc.wait()
    
async def ffmpeg_wav_to_mp3(file_in, file_out):
    proc = await asyncio.create_subprocess_exec("ffmpeg", "-y", "-i", file_in, "-q:a", "9", file_out,
        stdout=asyncio.subprocess.DEVNULL,
        stderr=asyncio.subprocess.DEVNULL)
    
    await proc.wait()
    
async def ffmpeg_wav_to_wav(file_in, file_out):
# Settings to reencode from:
# https://stackoverflow.com/questions/39668146/ffmpeg-making-a-clean-wav-file
    proc = await asyncio.create_subprocess_exec("ffmpeg", "-y",
        "-i", file_in,
        "-f", "wav",
        "-bitexact",
        "-acodec", "pcm_s16le",
        file_out,
        stdout=asyncio.subprocess.DEVNULL,
        stderr=asyncio.subprocess.DEVNULL)
    
    await proc.wait()
    
async def trash_process(file_name):
    proc = await asyncio.create_subprocess_exec("rm", file_name,
        stdout=asyncio.subprocess.DEVNULL,
        stderr=asyncio.subprocess.DEVNULL)
    
    await proc.wait()
    
async def fxbassboost(file_in, file_out):
    proc = await asyncio.create_subprocess_exec("audiofx/fx_bassboost", file_in, file_out,
        stdout=sys.stdout, # For debugging purposes, the fx will just print its output
        stderr=asyncio.subprocess.STDOUT)
    
    await proc.wait() 
    
async def fxvideo(file_in, file_out):
    proc = await asyncio.create_subprocess_exec("vidfx/vidfx", file_in, file_out,
        stdout=sys.stdout, # For debugging purposes, the fx will just print its output
        stderr=asyncio.subprocess.STDOUT)
    
    await proc.wait() 
        
        
async def file_deletion_loop(trash_queue):
    while True:
        file = await trash_queue.get()
        
        await trash_process(file)
        
        trash_queue.task_done()
        
    
@bot.event
async def on_ready():
    print("Bot is online")
    
    asyncio.create_task(file_deletion_loop(trash_queue))

bot.run(DISCORD_TOKEN)